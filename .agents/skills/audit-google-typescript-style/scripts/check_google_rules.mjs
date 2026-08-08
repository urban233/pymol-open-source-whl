#!/usr/bin/env node

import fs from 'node:fs';
import path from 'node:path';
import {createRequire} from 'node:module';

function parseArguments(argv) {
  const options = {root: process.cwd()};
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    if (argument === '--root') {
      options.root = path.resolve(argv[index + 1] ?? '');
      index += 1;
    } else if (argument === '--help' || argument === '-h') {
      console.log('Usage: node check_google_rules.mjs [--root package-root] (audits package-root/src)');
      process.exit(0);
    } else {
      throw new Error(`Unknown argument: ${argument}`);
    }
  }
  return options;
}

function loadTypeScript(root) {
  try {
    return createRequire(path.join(root, 'package.json'))('typescript');
  } catch (error) {
    throw new Error(`Could not resolve TypeScript from ${root}.`, {cause: error});
  }
}

function isExcluded(filePath, root) {
  const segments = path.relative(root, filePath).split(path.sep).map((segment) => segment.toLowerCase());
  const excluded = new Set(['.git', 'node_modules', '.vscode-test', 'dist', 'out', 'build', 'coverage', 'vendor', 'generated', 'gen']);
  if (segments.some((segment) => excluded.has(segment))) return true;
  const basename = path.basename(filePath).toLowerCase();
  return basename.includes('.generated.') || basename.includes('.gen.');
}

function collectSourceFiles(directory, root, files = []) {
  for (const entry of fs.readdirSync(directory, {withFileTypes: true})) {
    const entryPath = path.join(directory, entry.name);
    if (isExcluded(entryPath, root)) continue;
    if (entry.isDirectory()) collectSourceFiles(entryPath, root, files);
    else if (/\.(ts|tsx)$/u.test(entry.name)) files.push(entryPath);
  }
  return files;
}

function addFinding(findings, sourceFile, node, rule, level, message, root) {
  const position = sourceFile.getLineAndCharacterOfPosition(node.getStart(sourceFile));
  findings.push({level, rule, file: path.relative(root, sourceFile.fileName).split(path.sep).join('/'), line: position.line + 1, column: position.character + 1, message});
}

function hasExportModifier(node, ts) {
  return Boolean(ts.getModifiers(node)?.some((modifier) => modifier.kind === ts.SyntaxKind.ExportKeyword));
}

function hasDefaultModifier(node, ts) {
  return Boolean(ts.getModifiers(node)?.some((modifier) => modifier.kind === ts.SyntaxKind.DefaultKeyword));
}

function hasLeadingJsDoc(sourceFile, node, ts) {
  const comments = ts.getLeadingCommentRanges(sourceFile.text, node.getFullStart()) ?? [];
  return comments.some((comment) => sourceFile.text.slice(comment.pos, comment.end).startsWith('/**'));
}

function isDocumentableDeclaration(node, ts) {
  return ts.isFunctionDeclaration(node) || ts.isClassDeclaration(node) ||
    ts.isInterfaceDeclaration(node) || ts.isTypeAliasDeclaration(node) ||
    ts.isEnumDeclaration(node) || ts.isEnumMember(node) ||
    ts.isMethodDeclaration(node) || ts.isConstructorDeclaration(node) ||
    ts.isGetAccessorDeclaration(node) || ts.isSetAccessorDeclaration(node) ||
    ts.isPropertyDeclaration(node) || ts.isMethodSignature(node) ||
    ts.isCallSignatureDeclaration(node) || ts.isConstructSignatureDeclaration(node) ||
    ts.isPropertySignature(node) || ts.isIndexSignatureDeclaration(node);
}

function inspectSource(sourceFile, root, ts, findings) {
  for (const match of sourceFile.text.matchAll(/@ts-(?:ignore|nocheck|expect-error)/gu)) {
    const node = {getStart: () => match.index};
    addFinding(findings, sourceFile, node, 'typescript-suppression', match[0] === '@ts-expect-error' ? 'review' : 'violation', `${match[0]} requires a documented, narrowly scoped justification.`, root);
  }

  function visit(node) {
    if (ts.isExportAssignment(node) && !node.isExportEquals) addFinding(findings, sourceFile, node, 'no-default-export', 'violation', 'Use a named export instead of a default export.', root);
    if (hasDefaultModifier(node, ts)) addFinding(findings, sourceFile, node, 'no-default-export', 'violation', 'Use a named export instead of a default export.', root);
    if (ts.isVariableStatement(node)) {
      if ((node.declarationList.flags & ts.NodeFlags.Var) !== 0) addFinding(findings, sourceFile, node, 'no-var', 'violation', 'Use const or let instead of var.', root);
      if (hasExportModifier(node, ts) && (node.declarationList.flags & ts.NodeFlags.Let) !== 0) addFinding(findings, sourceFile, node, 'no-mutable-export', 'violation', 'Do not use export let.', root);
    }
    if (isDocumentableDeclaration(node, ts) && !hasLeadingJsDoc(sourceFile, node, ts)) addFinding(findings, sourceFile, node, 'symbol-jsdoc', 'review', 'Add JSDoc with a summary and applicable parameter, return, and error documentation for this public or private symbol.', root);
    if (ts.isImportEqualsDeclaration(node) && ts.isExternalModuleReference(node.moduleReference)) addFinding(findings, sourceFile, node, 'no-import-equals-require', 'violation', 'Use ES module import syntax instead of import equals require.', root);
    if (ts.isEnumDeclaration(node) && (ts.getCombinedModifierFlags(node) & ts.ModifierFlags.Const) !== 0) addFinding(findings, sourceFile, node, 'no-const-enum', 'violation', 'Use a plain enum instead of const enum.', root);
    if (ts.isModuleDeclaration(node)) {
      const declarationText = sourceFile.text.slice(node.getStart(sourceFile), node.getStart(sourceFile) + 32);
      if (/^(?:export\s+)?(?:declare\s+)?(?:namespace|module)\b/u.test(declarationText)) addFinding(findings, sourceFile, node, 'no-namespace', 'violation', 'Use ES modules instead of namespaces or internal modules.', root);
    }
    if (ts.isDebuggerStatement(node)) addFinding(findings, sourceFile, node, 'no-debugger', 'violation', 'Remove debugger statements from production code.', root);
    if (ts.isWithStatement(node)) addFinding(findings, sourceFile, node, 'no-with', 'violation', 'Do not use the with statement.', root);
    if (ts.isForInStatement(node)) addFinding(findings, sourceFile, node, 'for-in-filtering', 'review', 'Confirm that this for...in loop filters own properties.', root);
    if (ts.isNewExpression(node) || ts.isCallExpression(node)) {
      const expression = node.expression;
      if (ts.isIdentifier(expression) && ['Array', 'Object', 'String', 'Boolean', 'Number'].includes(expression.text)) addFinding(findings, sourceFile, node, `no-${expression.text.toLowerCase()}-constructor`, 'violation', `Do not use ${ts.isNewExpression(node) ? 'new ' : ''}${expression.text} as a constructor or coercion shortcut.`, root);
      if (ts.isIdentifier(expression) && ['eval', 'Function'].includes(expression.text)) addFinding(findings, sourceFile, node, 'no-dynamic-evaluation', 'violation', 'Do not use eval or the Function constructor.', root);
    }
    ts.forEachChild(node, visit);
  }
  visit(sourceFile);
}

function main() {
  const options = parseArguments(process.argv.slice(2));
  const ts = loadTypeScript(options.root);
  const sourceRoot = path.join(options.root, 'src');
  const files = fs.existsSync(sourceRoot) && fs.statSync(sourceRoot).isDirectory()
    ? collectSourceFiles(sourceRoot, options.root).sort()
    : [];
  const findings = [];
  for (const filePath of files) {
    const sourceFile = ts.createSourceFile(filePath, fs.readFileSync(filePath, 'utf8'), ts.ScriptTarget.Latest, true, filePath.endsWith('.tsx') ? ts.ScriptKind.TSX : ts.ScriptKind.TS);
    inspectSource(sourceFile, options.root, ts, findings);
  }
  for (const finding of findings) console.log(JSON.stringify(finding));
  const violations = findings.filter((finding) => finding.level === 'violation').length;
  const reviews = findings.filter((finding) => finding.level === 'review').length;
  console.error(`Checked ${files.length} TypeScript files: ${violations} violations, ${reviews} review items.`);
  process.exitCode = violations === 0 ? 0 : 1;
}

try {
  main();
} catch (error) {
  console.error(error instanceof Error ? error.message : String(error));
  process.exitCode = 2;
}
