#A* -------------------------------------------------------------------
#B* This file contains source code for the PyMOL computer program
#C* Copyright (c) Schrodinger, LLC.
#D* -------------------------------------------------------------------
#E* It is unlawful to modify or remove this copyright notice.
#F* -------------------------------------------------------------------
#G* Please see the accompanying LICENSE file for further information.
#H* -------------------------------------------------------------------
#I* Additional authors of this source file include:
#-*
#-*
#-*
#Z* -------------------------------------------------------------------

PROPERTY_AUTO = -1
PROPERTY_BOOLEAN = 1
PROPERTY_INT = 2
PROPERTY_FLOAT = 3
PROPERTY_COLOR = 5
PROPERTY_STRING = 6

if True:
    cmd = __import__("sys").modules["pymol.cmd"]
    from .cmd import _cmd, DEFAULT_ERROR, DEFAULT_SUCCESS, QuietException
    from .querying import get_color_index_from_string_or_list

    def _typecast(value, proptype, _self=cmd):
        if proptype == PROPERTY_FLOAT:
            return float(value)
        elif proptype == PROPERTY_INT:
            return int(value)

        # py3 (statement is always false for py2)
        if isinstance(value, bytes) and not isinstance(value, str):
            value = value.decode(errors='ignore')

        if proptype == PROPERTY_BOOLEAN:
            if isinstance(value, str):
                value = value.lower()
                if value == 'false':
                    value = False
                elif value in cmd.boolean_dict:
                    value = cmd.boolean_dict[value]
                else:
                    value = True
            else:
                value = bool(value)
        elif proptype == PROPERTY_STRING:
            value = str(value)
        elif proptype == PROPERTY_COLOR:
            value = get_color_index_from_string_or_list(value, _self=_self)
        return value

    def get_property(propname, name, state=0, quiet=1, _self=cmd):
        '''
DESCRIPTION

    Get an object-level property

ARGUMENTS

    propname = string: Name of the property

    name = string: Name of a single object

    state = int: Object state, 0 for all states, -1 for current state
    {default: 0}

    '''
        state, quiet = int(state)-1, int(quiet)
        r = DEFAULT_ERROR
        proptype = None
        propval = None
        if not len(str(propname)):
            return None
        try:
            _self.lock(_self)
            r = _self._cmd.get_property(_self._COb, propname, name, state, quiet)
            if isinstance(r, list):
                proptype = r[0]
                propval = r[1]
        except:
            return None
        finally:
            _self.unlock(None,_self)
        if not quiet:
            if propval and proptype == PROPERTY_COLOR:
                try:
                    propval = dict((k,v) for (v,k) in _self.get_color_indices())[propval]
                except:
                    pass
            if propval:
                print(" get_property: '%s' in object '%s' : %s"%(propname, name, propval))
            else:
                print(" get_property: '%s' in object '%s' not found"%(propname, name))

        return propval

    def get_property_list(object, state=0, quiet=1, _self=cmd):
        '''
DESCRIPTION

    Get all properties for an object (for a particular state) as a list

ARGUMENTS

    object = string: Name of a single object

    state = int: Object state, 0 for all states, -1 for current state
    {default: 0}
    '''
        state, quiet = int(state)-1, int(quiet)
        r = DEFAULT_ERROR
        try:
            _self.lock(_self)
            r = _self._cmd.get_property(_self._COb, None, object, state, quiet)
            if not quiet:
                print(" get_property_list: %s : %s"%(object, r))
        finally:
            _self.unlock(None,_self)
        return r

    def set_property(name, value, object='*', state=0, proptype=PROPERTY_AUTO, quiet=1, _self=cmd):
        '''
DESCRIPTION

    Set an object-level property

USAGE

    set_property name, value [, object [, state [, proptype ]]]

ARGUMENTS

    name = string: Name of the property

    value = str/int/float/bool: Value to be set

    object = string: Space separated list of objects or * for all objects
    {default: *}

    proptype = int: The type of the property, -1=auto, 1=bool, 2=int,
    3=float, 5=color, 6=str. Type -1 will detect int (digits only), float,
    and bool (true/false/yes/no). {default: -1}

    state = int: Object state, 0 for all states, -1 for current state
    {default: 0}

EXAMPLE

    fragment ala
    set_property myfloatprop, 1234, ala, proptype=3
    get_property myfloatprop, ala

SEE ALSO

    get_property, get_property_list, set_atom_property
    '''
        state, quiet = int(state)-1, int(quiet)
        proptype = int(proptype)
        value = _typecast(value, proptype, _self)
        r = DEFAULT_ERROR
        try:
            _self.lock(_self)
            r = _self._cmd.set_property(_self._COb, name, value, object, proptype, state, quiet)
        finally:
            _self.unlock(r,_self)
        if _self._raising(r,_self): raise QuietException
        return r

    def set_atom_property(name, value, selection='all', state=0, proptype=PROPERTY_AUTO, quiet=1, _self=cmd):
        '''
DESCRIPTION

    Set an atom-level property

USAGE

    set_atom_property name, value [, selection [, state [, proptype ]]]

ARGUMENTS

    name = string: Name of the property

    value = str/int/float/bool: Value to be set

    selection = string: a selection-expression
    {default: all}

    proptype = int: The type of the property, -1=auto, 1=bool, 2=int,
    3=float, 5=color, 6=str. Type -1 will detect int (digits only), float,
    and bool (true/false/yes/no). {default: -1}

    state = int: Object state, 0 for all states, -1 for current state
    {default: 0}

EXAMPLE

    set_atom_property myfloatprop,  1.23, elem C
    set_atom_property myfloatprop,  1234, elem N, proptype=3
    set_atom_property myboolprop,   TRUE, elem O
    set_atom_property mystrprop,   false, elem O, proptype=6
    set_atom_property mystrprop, One Two, elem C
    iterate all, print(elem, p.all)
    alter all, p.myboolprop = True
    alter all, p.myfloatprop = None # clear

SEE ALSO

    set_property, iterate, alter
    '''
        state, quiet = int(state)-1, int(quiet)
        proptype = int(proptype)
        value = _typecast(value, proptype, _self)
        with _self.lockcm:
            r = _self._cmd.set_atom_property(_self._COb, name, value, selection, proptype, state, quiet)
        return r
