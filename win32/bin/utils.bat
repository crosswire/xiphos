echo off
rem ~dp0 gets the directory of this file.
set PATH=%~dp0;%PATH%
cd %HOMEPATH%
echo ------------------------------------------
echo addld: Lexicon and Dictionary creation tool
echo diatheke: multi-purpose command-line frontend
echo imp2gbs: General Book creation tool
echo imp2ld: Lexicon and Dictionary creation tool
echo imp2vs: VPL Bible/Commentary importer
echo installmgr: command-line module installer
echo mkfastmod: command-line module indexer
echo mod2imp: export module to imp format
echo mod2osis: export module to osis format
echo mod2zmod: compress a sword module
echo osis2mod: create Bible/Commentary/General Book from OSIS
echo tei2mod: create Lexicon/Dictionary from TEI
echo uconv: convert files from one character encoding to another
echo vpl2mod: import from VPL file
echo vs2osisreftxt: return an OSIS reference for any reference
echo xml2gbs: import from OSIS/ThML to a General Book
echo for more information, tutorials, and explanations of file formats, see http://crosswire.org/wiki
echo ---------------------------------------
cmd