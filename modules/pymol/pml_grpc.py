"""
A gRPC server to allow remote control of PyMol.

Replaces the legacy XML-RPC implementation with a modern,
high-performance gRPC service.

Author: Greg Landrum (original XML-RPC), Martin Urban (updated for gRPC)
Date: September 2025
License: PyMOL
Requires:
    - grpcio, grpcio-tools, protobuf
    - Python with threading enabled
"""

import os
import tempfile
from concurrent import futures

import grpc
from pymol import cmd, cgo

import pymol_rpc_pb2
import pymol_rpc_pb2_grpc

# --- Global state ---
cgoDict = {}  # stores CGO objects by id
_server = None


# --- Helper functions ---

def _color_obj(obj_name, color_scheme: str):
  """Applies a color scheme to a molecular object."""
  if not color_scheme:
    return
  try:
    if color_scheme == 'std':
      cmd.color("magenta", f"({obj_name})", quiet=1)
      cmd.color("oxygen", f"(elem O and {obj_name})", quiet=1)
      cmd.color("nitrogen", f"(elem N and {obj_name})", quiet=1)
      cmd.color("sulfur", f"(elem S and {obj_name})", quiet=1)
      cmd.color("hydrogen", f"(elem H and {obj_name})", quiet=1)
      cmd.color("gray", f"(elem C and {obj_name})", quiet=1)
    else:
      cmd.color(color_scheme, obj_name, quiet=1)
  except Exception as e:
    print(f"Error applying color scheme '{color_scheme}': {e}")


def _make_alpha_section(transparent: bool, transparency: float):
  return [] if not transparent else [cgo.ALPHA, 1.0 - transparency]


# --- gRPC Servicer Implementation ---

class PyMolRPCServicer(pymol_rpc_pb2_grpc.PyMolRPCServicer):
  """Implements the PyMolRPC gRPC service."""

  def Ping(self, request, context):
    return pymol_rpc_pb2.StatusResponse(success=True, message="Pong")

  def Do(self, request, context):
    try:
      result = cmd.do(request.command)
      return pymol_rpc_pb2.CommandResponse(result=str(result) if result else "")
    except Exception as e:
      return pymol_rpc_pb2.CommandResponse(result=f"Error: {e}")

  def Label(self, request, context):
    try:
      pos = (request.pos.x, request.pos.y, request.pos.z)
      color = (request.color.r, request.color.g, request.color.b)
      obj_id = request.id or 'lab1'
      color_id = f"{obj_id}-color"

      cmd.pseudoatom(obj_id, label=repr(request.text), elem='C', pos=pos)
      cmd.set_color(color_id, color)
      cmd.color(color_id, obj_id)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Sphere(self, request, context):
    try:
      pos = (request.pos.x, request.pos.y, request.pos.z)
      color = (request.color.r, request.color.g, request.color.b)
      obj_id = request.id or 'cgo'

      obj = cgoDict.get(obj_id, []).copy() if request.extend else []
      obj.extend(_make_alpha_section(request.transparent, request.transparency))
      obj.extend([cgo.COLOR, *color, cgo.SPHERE, *pos, request.radius])

      cgoDict[obj_id] = obj
      cmd.load_cgo(obj, obj_id, 1)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Cylinder(self, request, context):
    try:
      p1 = (request.end1.x, request.end1.y, request.end1.z)
      p2 = (request.end2.x, request.end2.y, request.end2.z)
      c1 = (request.color1.r, request.color1.g, request.color1.b)

      if request.HasField("color2"):
        c2 = (request.color2.r, request.color2.g, request.color2.b)
      else:
        c2 = c1

      obj_id = request.id or 'cgo'
      obj = cgoDict.get(obj_id, []).copy() if request.extend else []
      obj.extend(_make_alpha_section(request.transparent, request.transparency))
      obj.extend([cgo.CYLINDER, *p1, *p2, request.radius, *c1, *c2])

      cgoDict[obj_id] = obj
      cmd.load_cgo(obj, obj_id, 1)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def LoadPDB(self, request, context):
    if request.replace:
      cmd.delete(request.obj_name)
    result = cmd.read_pdbstr(request.data, request.obj_name)
    if request.HasField("color_scheme"):
      _color_obj(request.obj_name, request.color_scheme)
    return pymol_rpc_pb2.CommandResponse(result=str(result) if result else "")

  def LoadMolBlock(self, request, context):
    if request.replace:
      cmd.delete(request.obj_name)
    result = cmd.read_molstr(request.data, request.obj_name)
    if request.HasField("color_scheme"):
      _color_obj(request.obj_name, request.color_scheme)
    return pymol_rpc_pb2.CommandResponse(result=str(result) if result else "")

  def LoadFile(self, request, context):
    obj_name = request.obj_name or os.path.splitext(os.path.basename(request.file_name))[0]
    if request.replace:
      cmd.delete(obj_name)
    fmt = request.format if request.HasField("format") else ''
    result = cmd.load(request.file_name, obj_name, format=fmt)
    if request.HasField("color_scheme"):
      _color_obj(obj_name, request.color_scheme)
    return pymol_rpc_pb2.CommandResponse(result=str(result) if result else "")

  def LoadSurface(self, request, context):
    grid_name = f"grid-{request.obj_name}"
    file_to_load = None

    try:
      if request.WhichOneof("source") == "data":
        with tempfile.NamedTemporaryFile(delete=False, mode='w', suffix='.grd') as tmp:
          tmp.write(request.data)
          file_to_load = tmp.name
      elif request.WhichOneof("source") == "file_name":
        file_to_load = request.file_name
      else:
        return pymol_rpc_pb2.CommandResponse(result="Error: no source specified")

      fmt = request.format if request.HasField("format") else ''
      result = cmd.load(file_to_load, grid_name, format=fmt)
      cmd.isosurface(request.obj_name, grid_name, level=request.surface_level)
      return pymol_rpc_pb2.CommandResponse(result=str(result) if result else "")
    finally:
      if file_to_load and file_to_load.endswith(".grd") and os.path.exists(file_to_load):
        os.unlink(file_to_load)

  def GetNames(self, request, context):
    names = cmd.get_names(request.what, enabled_only=request.enabled_only)
    return pymol_rpc_pb2.GetNamesResponse(names=names)

  def GetAtomCoords(self, request, context):
    coords = cmd.get_atom_coords(request.selection, state=request.state)
    if coords is None:
      return pymol_rpc_pb2.GetAtomCoordsResponse(coordinates=[])
    response_coords = [pymol_rpc_pb2.Vector3(x=c[0], y=c[1], z=c[2]) for c in coords]
    return pymol_rpc_pb2.GetAtomCoordsResponse(coordinates=response_coords)

  def DeleteObject(self, request, context):
    try:
      cmd.delete(request.name)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def DeleteAll(self, request, context):
    try:
      cmd.delete("all")
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Center(self, request, context):
    try:
      cmd.center(request.selection, animate=int(request.animate))
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Zoom(self, request, context):
    try:
      cmd.zoom(request.selection, buffer=request.buffer,
               state=request.state, animate=int(request.animate))
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Rotate(self, request, context):
    try:
      cmd.rotate(request.axis, request.angle, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Move(self, request, context):
    try:
      cmd.move(request.axis, request.distance, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Hide(self, request, context):
    try:
      cmd.hide(request.representation, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Show(self, request, context):
    try:
      cmd.show(request.representation, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Color(self, request, context):
    try:
      cmd.color(request.color, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def SetTransparency(self, request, context):
    try:
      cmd.set("transparency", request.transparency, request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def Select(self, request, context):
    try:
      cmd.select("sele", request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

  def DeleteSelection(self, request, context):
    try:
      cmd.delete(request.selection)
      return pymol_rpc_pb2.StatusResponse(success=True)
    except Exception as e:
      return pymol_rpc_pb2.StatusResponse(success=False, message=str(e))

# --- Server Launching Logic ---

def launch_gRPC(hostname='', port=50051, n_to_try=5):
  """Launches the gRPC server in a background thread."""
  global _server
  if _server is not None:
    print("gRPC server already running.")
    return

  if not hostname:
    hostname = os.environ.get('PYMOL_RPCHOST', 'localhost')

  server_port = None
  for i in range(n_to_try):
    current_port = port + i
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    pymol_rpc_pb2_grpc.add_PyMolRPCServicer_to_server(PyMolRPCServicer(), server)
    bound = server.add_insecure_port(f"{hostname}:{current_port}")
    if bound == 0:
      print(f"Port {current_port} unavailable; trying next")
      continue
    server.start()
    _server = server
    server_port = current_port
    break

  if _server:
    print(f"gRPC server running on host {hostname}, port {server_port}")
  else:
    print("gRPC server could not be started")
