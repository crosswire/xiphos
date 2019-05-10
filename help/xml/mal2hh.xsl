<?xml version="1.0"?>
<!DOCTYPE xsl:stylesheet [
<!ENTITY lf '<xsl:text xmlns:xsl="http://www.w3.org/1999/XSL/Transform">&#xA;</xsl:text>'>
]>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:mal="http://projectmallard.org/1.0/"


                xmlns:html="http://www.w3.org/1999/xhtml"
                xmlns:mml="http://www.w3.org/1998/Math/MathML"
                xmlns:svg="http://www.w3.org/2000/svg"
		xmlns:exsl="http://exslt.org/common"

                xmlns:redirect="http://xml.apache.org/xalan/redirect"
                xmlns:saxon="http://icl.com/saxon"
		xmlns:cache="http://projectmallard.org/cache/1.0/"
                exclude-result-prefixes="cache html mml svg mal exsl redirect saxon"
		extension-element-prefixes="exsl"
                version="1.0" >



<!--
Mallard to CHM - Microsoft HTML Help Project file (.hhp)
Handle simple Mallard block elements.
:Revision:version="0.1" date="2018-06-16" status="draft"

This stylesheet contains templates for creating the .HHP file.
-->


<!--!!==========================================================================
    HHP vars
-->

<!-- html.extension - The filename extension for all output files. This
     parameter specifies a filename extension for all HTML output files. It
     should include the leading dot. -->
<xsl:param name="html.extension">.html</xsl:param>

<!-- htmlhelp.hhp — Filename of project file
     Change this parameter if you want different name of project file than
     htmlhelp.hhp. -->
<xsl:param name="htmlhelp.hhp">htmlhelp.hhp</xsl:param>

<!-- htmlhelp.langID — . -->
<xsl:param name="htmlhelp.langID"></xsl:param>

<!-- htmlhelp.encoding — Character encoding to use in files for HTML Help
     compiler. The HTML Help Compiler is not UTF-8 aware, so you should always
     use an appropriate single-byte encoding here. -->
<xsl:param name="htmlhelp.encoding"></xsl:param>

<!-- htmlhelp.title — Title of HTML Help. Content of this parameter will be used
     as a title for generated HTML Help. If empty, title should be automatically
     taken from document. -->
<xsl:param name="htmlhelp.title"></xsl:param>

<!-- htmlhelp.default.topic — Name of file with default topic. Normally first
     page displayed when you open HTML Help file, default is index.html. If you
     want to display another topic, simply set its filename by this parameter.
     -->
<!-- <xsl:param name="htmlhelp.default.topic">index.html</xsl:param> -->
<xsl:param name="htmlhelp.default.topic"></xsl:param>

<!-- htmlhelp.show.menu — Should the menu bar be shown? Set to non-zero to have
     an application menu bar in your HTML Help window. -->
<xsl:param name="htmlhelp.show.menu" select="0"/>

<!-- htmlhelp.show.advanced.search — Should advanced search features be
     available? f you want advanced search features in your help, turn this
     parameter to 1. -->
<xsl:param name="htmlhelp.show.advanced.search" select="0"/>

<!-- htmlhelp.show.favorities — Should the Favorites tab be shown? Set to
     non-zero to include a Favorites tab in the navigation pane of the help
     window. -->
<xsl:param name="htmlhelp.show.favorities" select="0"/>

<!-- htmlhelp.show.toolbar.text — Show text under toolbar buttons? Set to
     non-zero to display texts under toolbar buttons, zero to switch off
     displays. -->
<xsl:param name="htmlhelp.show.toolbar.text" select="1"/>

<!-- htmlhelp.remember.window.position — Remember help window position? Set to
     non-zero to remember help window position between starts. -->
<xsl:param name="htmlhelp.remember.window.position" select="0"/>

<!-- htmlhelp.button.hideshow — Should the Hide/Show button be shown? Set to
     non-zero to include the Hide/Show button shown on toolbar -->
<xsl:param name="htmlhelp.button.hideshow" select="1"/>

<!-- htmlhelp.button.back — Should the Back button be shown? Set to non-zero
     to include the Hide/Show button shown on toolbar -->
<xsl:param name="htmlhelp.button.back" select="1"/>

<!-- htmlhelp.button.forward — Should the Forward button be shown? Set to
     non-zero to include the Forward button on the toolbar. -->
<xsl:param name="htmlhelp.button.forward" select="0"/>

<!-- htmlhelp.button.stop — Should the Stop button be shown? If you want Stop
     button shown on toolbar, turn this parameter to 1. -->
<xsl:param name="htmlhelp.button.stop" select="0"/>

<!-- htmlhelp.button.refresh — Should the Refresh button be shown? Set to
     non-zero to include the Stop button on the toolbar. -->
<xsl:param name="htmlhelp.button.refresh" select="0"/>

<!-- htmlhelp.button.home — Should the Home button be shown? Set to non-zero to
     include the Home button on the toolbar. -->
<xsl:param name="htmlhelp.button.home" select="0"/>

<!-- htmlhelp.button.options — Should the Options button be shown? If you want
     Options button shown on toolbar, turn this parameter to 1. -->
<xsl:param name="htmlhelp.button.options" select="1"/>

<!-- htmlhelp.button.print — Should the Print button be shown? Set to non-zero
     to include the Print button on the toolbar. -->
<xsl:param name="htmlhelp.button.print" select="1"/>

<!-- htmlhelp.button.locate — Should the Locate button be shown? If you want
     Locate button shown on toolbar, turn this parameter to 1. -->
<xsl:param name="htmlhelp.button.locate" select="0"/>

<!-- htmlhelp.button.jump1 — Should the Jump1 button be shown? Set to non-zero
     to include the Jump1 button on the toolbar. -->
<xsl:param name="htmlhelp.button.jump1" select="0"/>

<!-- htmlhelp.button.jump2 — Should the Jump2 button be shown? Set to non-zero
     to include the Jump2 button on the toolbar. -->
<xsl:param name="htmlhelp.button.jump2" select="0"/>

<!-- htmlhelp.button.next — Should the Next button be shown? Set to non-zero
     to include the Next button on the toolbar. -->
<xsl:param name="htmlhelp.button.next" select="1"/>

<!-- tmlhelp.button.prev — Should the Prev button be shown? Set to non-zero
     to include the Prev button on the toolbar. -->
<xsl:param name="htmlhelp.button.prev" select="1"/>

<!-- htmlhelp.button.zoom — Should the Zoom button be shown? Set to non-zero
     to include the Zoom button on the toolbar. -->
<xsl:param name="htmlhelp.button.zoom" select="0"/>

<!-- htmlhelp.button.home.url — URL address of page accessible by Home button
     URL address of page accessible by Home button. -->
<xsl:param name="htmlhelp.button.home.url"/>

<!-- htmlhelp.button.jump1.title — Title of Jump1 button -->
<xsl:param name="htmlhelp.button.jump1.title">User1</xsl:param>

<!-- htmlhelp.button.jump1.url — URL address of page accessible by Jump1 button
-->
<xsl:param name="htmlhelp.button.jump1.url"/>

<!-- htmlhelp.button.jump2.title — Title of Jump2 button -->
<xsl:param name="htmlhelp.button.jump2.title">User2</xsl:param>

<!-- htmlhelp.button.jump2.url — URL address of page accessible by Jump2 button
-->
<xsl:param name="htmlhelp.button.jump2.url"/>

<!-- Generating an index file (index.hhk). Set to 1 if you want to generate an
     index file (Note: Not implemented actually) -->
<xsl:param name="htmlhelp.generate.index" select="0"/>

<!-- htmlhelp.hhp.window — Name of default window. Name of default window. If
     empty no [WINDOWS] section will be added to project file. -->
<xsl:param name="htmlhelp.hhp.window">Main</xsl:param>

<!--  htmlhelp.hhc.binary — Generate binary ToC? Set to non-zero to generate a
     binary TOC. You must create a binary TOC if you want to add Prev/Next
     buttons to toolbar (which is default behaviour). Files with binary TOC
     can't be merged. -->
<xsl:param name="htmlhelp.hhc.binary" select="1"/>

<!-- htmlhelp.chm — Filename of output HTML Help file. -->
<xsl:param name="htmlhelp.chm">htmlhelp.chm</xsl:param>

<!-- htmlhelp.hhc — Filename of TOC file. Set the name of the TOC file. The
     default is toc.hhc. -->
<xsl:param name="htmlhelp.hhc">toc.hhc</xsl:param>

<!--  htmlhelp.display.progress — Display compile progress? Set to non-zero
     to to display compile progress -->
<xsl:param name="htmlhelp.display.progress" select="1"/>

<!-- htmlhelp.enhanced.decompilation — Allow enhanced decompilation of CHM? When
     non-zero this parameter enables enhanced decompilation of CHM. -->
<xsl:param name="htmlhelp.enhanced.decompilation" select="0"/>

<!--  htmlhelp.hhc.width — Width of navigation pane. This parameter specifies
     the width of the navigation pane (containing TOC and other navigation
     tabs) in pixels. -->
<xsl:param name="htmlhelp.hhc.width"/>

<!-- htmlhelp.window.geometry — Set initial geometry of help window. This
     parameter specifies initial position of help window.
     E.g.>[160,64,992,704]< -->
<xsl:param name="htmlhelp.window.geometry"/>

<!-- htmlhelp.hhp.windows — Definition of additional windows. Content of this
     parameter is placed at the end of [WINDOWS] section of project file. You
     can use it for defining your own addtional windows. -->
<xsl:param name="htmlhelp.hhp.windows"/>

<!-- htmlhelp.enumerate.images — Should the paths to all used images be added to
     the project file? Set to non-zero if you insert images into your documents
     as external binary entities or if you are using absolute image paths. -->
<xsl:param name="htmlhelp.enumerate.images" select="0"/>

<!-- htmlhelp.force.map.and.alias — Should [MAP] and [ALIAS] sections be added
     to the project file unconditionally? Set to non-zero if you have your own
     alias.h and context.h files and you want to include references to them in
     the project file. -->
<xsl:param name="htmlhelp.force.map.and.alias" select="0"/>

<!-- htmlhelp.hhp.tail — Additional content for project file. If you want to
     include some additional parameters into project file, store appropriate
     part of project file into this parameter. -->
<xsl:param name="htmlhelp.hhp.tail"/>

<!-- htmlhelp.hhk — Filename of index file. Set the name of the index file.
     The default is index.hhk. -->
<xsl:param name="htmlhelp.hhk">index.hhk</xsl:param>

<!-- htmlhelp.map.file — Filename of map file. Set the name of map file. The
     default is context.h. (used for context-sensitive help). -->
<xsl:param name="htmlhelp.map.file">context.h</xsl:param>

<!--  htmlhelp.alias.file — Filename of alias file. Specifies the filename of
     the alias file (used for context-sensitive help). -->
<xsl:param name="htmlhelp.alias.file">alias.h</xsl:param>

<!-- htmlhelp.hhc.section.depth — Depth of TOC for sections in a left pane. Set
     the section depth in the left pane of HTML Help viewer. -->
<xsl:param name="htmlhelp.hhc.section.depth">5</xsl:param>

<!-- htmlhelp.hhc.show.root — Should there be an entry for the root element in
     the ToC? If set to zero, there will be no entry for the root element in the
     ToC. This is useful when you want to provide the user with an expanded ToC
     as a default. -->
<xsl:param name="htmlhelp.hhc.show.root" select="1"></xsl:param>

<!-- htmlhelp.hhc.folders.instead.books — Use folder icons in ToC (instead of
     book icons)? Set to non-zero for folder-like icons or zero for book-like
     icons in the ToC. If you want to use folder-like icons, you must switch off
     the binary ToC using htmlhelp.hhc.binary. -->
<xsl:param name="htmlhelp.hhc.folders.instead.books" select="0"></xsl:param>

<!-- TODO: NOT USED ACTUALLY ? -->
<!-- htmlhelp.use.hhk — Should the index be built using the HHK file? If
     non-zero, the index is created using the HHK file (instead of using object
     elements in the HTML files). -->
<!-- <xsl:param name="htmlhelp.use.hhk" select="0"></xsl:param> -->

<!-- htmlhelp.hhc — Filename of TOC file. Set the name of the TOC file. The
     default is toc.hhc. -->
<xsl:param name="htmlhelp.hhc">toc.hhc</xsl:param>

<!-- chunk.quietly — Omit the chunked filename messages. If zero (the default),
     the XSL processor emits a message naming each separate chunk filename as it
     is being output. If nonzero, then the messages are suppressed. -->
<xsl:param name="chunk.quietly" select="0"/>

<!-- exsl:document | method = { "xml" | "html" | "text" } -->
<xsl:param name="chunker.output.method" select="'html'"/>

<!-- exsl:document | omit-xml-declaration = { "yes" | "no" } -->
<xsl:param name="chunker.output.omit-xml-declaration" select="'no'"/>

<!-- exsl:document | standalone = { "yes" | "no" } -->
<xsl:param name="chunker.output.standalone" select="'no'"/>

<!-- exsl:document | doctype-public -->
<xsl:param name="chunker.output.doctype-public" select="''"/>

<!-- exsl:document | doctype-system -->
<xsl:param name="chunker.output.doctype-system" select="''"/>

<!-- exsl:document | cdata-section-elements -->
<xsl:param name="chunker.output.cdata-section-elements" select="''"/>

<!-- exsl:document | indent = { "yes" | "no" } -->
<xsl:param name="chunker.output.indent" select="non" />

<!-- exsl:document | media-type  -->
<xsl:param name="chunker.output.media-type" select="''"/>



<!--!!==========================================================================
main
:Revision:version="0.2" date="2018-07-15" status="draft"

Mallard to CHM - Mallard to Microsoft HTML Help Project files conversion
This section contains templates for creating the htmlhelp project files.
-->

<!-- select pages by type -->
<xsl:key name="page-type" match="cache:cache/mal:page" use="@type" />

<!-- select pages by id -->
<xsl:key name="page-id" match="cache:cache/mal:page/mal:info/mal:link" use="@xref" />

<xsl:template match="/">
  <xsl:call-template name="hhp" />
  <xsl:call-template name="hhc" />
</xsl:template>


<!--!!==========================================================================
hhp
Create the project file
:Revision:version="0.7" date="2018-08-07" status="draft"

This template creates the project file (.HHP)
-->

<xsl:template name="hhp">
  <xsl:call-template name="write.chunk">
    <xsl:with-param name="filename">
      <xsl:value-of select="$htmlhelp.hhp"/>
    </xsl:with-param>
    <xsl:with-param name="suppress-context-node-name" select="0"/>
    <xsl:with-param name="message-prolog"/>
    <xsl:with-param name="message-epilog"/>
    <xsl:with-param name="method" select="'text'"/>
    <xsl:with-param name="encoding" select="'utf-8'"/>
    <xsl:with-param name="indent" select="'no'"/>
    <xsl:with-param name="omit-xml-declaration" select="'yes'"/>
    <xsl:with-param name="standalone" select="'no'"/>
    <xsl:with-param name="doctype-public"/>
    <xsl:with-param name="doctype-system"/>
    <xsl:with-param name="media-type" select="$chunker.output.media-type"/>
    <xsl:with-param name="cdata-section-elements"/>
    <xsl:with-param name="content">
      <xsl:call-template name="hhp-main"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>


<!-- ====================================================================
hhp-main
Configure the project file
:Revision:version="0.3" date="2018-08-07" status="draft"

This template sets variables included into the project file (.HHP)
-->

<xsl:template name="hhp-main">

  <!-- Set Help Title - If empty, we use the title of the first 'guide' page  -->
  <xsl:variable name="raw.help.title">
    <xsl:choose>
      <xsl:when test="$htmlhelp.title = ''">
	<xsl:for-each select="key('page-type','guide')">
	  <xsl:value-of select="mal:title[1]/text()[last()]" />
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$htmlhelp.title"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="help.title" select="normalize-space($raw.help.title)"/>


  <!-- set Default Topic - If empty we use the id of the first 'guide' page-->
  <xsl:variable name="default.topic">
    <xsl:choose>
      <xsl:when test="$htmlhelp.default.topic != ''">
	<xsl:value-of select="$htmlhelp.default.topic"/>
      </xsl:when>
      <xsl:otherwise>
      	<xsl:for-each select="key('page-type','guide')">
	  <xsl:value-of select="concat(@id[1],$html.extension)" />
	</xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>


  <!-- Set navigation variable -->
  <xsl:variable name="xnavigation">
    <xsl:text>0x</xsl:text>
    <xsl:call-template name="ConvertDecToHex">
      <xsl:with-param name="index" select="9504 + $htmlhelp.show.menu * 65536
                                           + $htmlhelp.show.advanced.search * 131072
                                           + $htmlhelp.show.favorities * 4096
                                           + (1 - $htmlhelp.show.toolbar.text) * 64
                                           + $htmlhelp.remember.window.position * 262144"/>
    </xsl:call-template>
  </xsl:variable>

  <!-- Set buttons variable -->
  <xsl:variable name="xbuttons">
    <xsl:text>0x</xsl:text>
    <xsl:call-template name="ConvertDecToHex">
      <xsl:with-param name="index" select="0 + $htmlhelp.button.hideshow * 2
					   + $htmlhelp.button.back * 4
					   + $htmlhelp.button.forward * 8
					   + $htmlhelp.button.stop * 16
					   + $htmlhelp.button.refresh * 32
					   + $htmlhelp.button.home * 64
					   + $htmlhelp.button.options * 4096
					   + $htmlhelp.button.print * 8192
					   + $htmlhelp.button.locate * 2048
					   + $htmlhelp.button.jump1 * 262144
					   + $htmlhelp.button.jump2 * 524288
					   + $htmlhelp.button.next * 2097152
					   + $htmlhelp.button.prev * 4194304
					   + $htmlhelp.button.zoom * 1048576"/>
    </xsl:call-template>
  </xsl:variable>

  <!-- Section1: Options -->
  <xsl:text>[OPTIONS]
</xsl:text>

  <!-- auto index -->
  <xsl:if test="$htmlhelp.generate.index">
    <xsl:text>Auto Index=Yes
</xsl:text>
  </xsl:if>

  <!-- Binary TOC -->
  <xsl:if test="$htmlhelp.hhc.binary != 0">
    <xsl:text>Binary TOC=Yes
</xsl:text>
  </xsl:if>

  <!-- Compatibility -->
  <xsl:text>Compatibility=1.1 or later
Compiled file=</xsl:text><xsl:value-of select="$htmlhelp.chm"/><xsl:text>
Contents file=</xsl:text><xsl:value-of select="$htmlhelp.hhc"/><xsl:text>
</xsl:text>

  <!-- default window -->
  <xsl:if test="$htmlhelp.hhp.window != ''">
    <xsl:text>Default Window=</xsl:text>
    <xsl:value-of select="$htmlhelp.hhp.window"/><xsl:text>
</xsl:text>
  </xsl:if>

  <!-- Default Topic -->
  <xsl:text>Default topic=</xsl:text>
  <xsl:value-of select="$default.topic"/>

  <!-- Show compile progress -->
   <xsl:text>
Display compile progress=</xsl:text>
  <xsl:choose>
    <xsl:when test="$htmlhelp.display.progress != 1">
      <xsl:text>No</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>Yes</xsl:text>
    </xsl:otherwise>
  </xsl:choose>

  <!-- Full text search -->
  <xsl:text>
Full-text search=Yes
</xsl:text>

  <!-- index file -->
  <xsl:if test="$htmlhelp.generate.index">
    <xsl:text>Index file=</xsl:text>
    <xsl:value-of select="$htmlhelp.hhk"/><xsl:text>
</xsl:text>
  </xsl:if>

  <!-- Language -->
  <xsl:text>Language=</xsl:text>
  <xsl:call-template name="set-langID" />
  <!-- Title -->
  <xsl:text>
Title=</xsl:text>
  <xsl:value-of select="$help.title"/>

  <!-- Enhanced decompilation -->
<xsl:text>
Enhanced decompilation=</xsl:text>
  <xsl:choose>
    <xsl:when test="$htmlhelp.enhanced.decompilation != 0">
      <xsl:text>Yes</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>No</xsl:text>
    </xsl:otherwise>
  </xsl:choose>


  <!-- Section2: Windows -->
<xsl:if test="$htmlhelp.hhp.window != ''">
  <xsl:text>

[WINDOWS]
</xsl:text>
  <xsl:value-of select="$htmlhelp.hhp.window"/>
  <xsl:text>="</xsl:text>
  <xsl:value-of select="$help.title"/>
  <xsl:text>","</xsl:text>
  <xsl:value-of select="$htmlhelp.hhc"/>
  <xsl:text>",</xsl:text>
  <xsl:if test="$htmlhelp.generate.index">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="$htmlhelp.hhk"/>
    <xsl:text>"</xsl:text>
  </xsl:if>
  <xsl:text>,"</xsl:text>
  <xsl:value-of select="$default.topic"/>
  <xsl:text>",</xsl:text>
  <xsl:text>"</xsl:text>
  <xsl:choose>
    <xsl:when test="$htmlhelp.button.home != 0">
      <xsl:value-of select="$htmlhelp.button.home.url"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$default.topic"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>"</xsl:text>
  <xsl:text>,</xsl:text>
  <xsl:if test="$htmlhelp.button.jump1 != 0">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="$htmlhelp.button.jump1.url"/>
    <xsl:text>"</xsl:text>
  </xsl:if>
  <xsl:text>,</xsl:text>
  <xsl:if test="$htmlhelp.button.jump1 != 0">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="$htmlhelp.button.jump1.title"/>
    <xsl:text>"</xsl:text>
  </xsl:if>
  <xsl:text>,</xsl:text>
  <xsl:if test="$htmlhelp.button.jump2 != 0">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="$htmlhelp.button.jump2.url"/>
    <xsl:text>"</xsl:text>
  </xsl:if>
  <xsl:text>,</xsl:text>
  <xsl:if test="$htmlhelp.button.jump2 != 0">
    <xsl:text>"</xsl:text>
    <xsl:value-of select="$htmlhelp.button.jump2.title"/>
    <xsl:text>"</xsl:text>
  </xsl:if>
  <xsl:text>,</xsl:text>
  <xsl:value-of select="$xnavigation"/>
  <xsl:text>,</xsl:text><xsl:value-of select="$htmlhelp.hhc.width"/><xsl:text>,</xsl:text>
  <xsl:value-of select="$xbuttons"/>
  <xsl:text>,</xsl:text><xsl:value-of select="$htmlhelp.window.geometry"/><xsl:text>,,,,,,,0
</xsl:text>
  </xsl:if>

  <xsl:if test="$htmlhelp.hhp.windows">
    <xsl:value-of select="$htmlhelp.hhp.windows"/>
  </xsl:if>

  <!-- Section3: Files -->
  <xsl:text>

[FILES]
</xsl:text>
  <!-- Files list -->
  <xsl:for-each select="cache:cache/mal:page">
    <xsl:variable name="id" select="@id"/>
    <xsl:value-of select="concat($id, '.html')"/>
    <xsl:text>
</xsl:text>
  </xsl:for-each>

  <!-- TODO! enumerate images and css files -->
  <xsl:if test="$htmlhelp.enumerate.images">
    <xsl:text>Not implemented yet!</xsl:text>
  </xsl:if>

  <!-- Section4: Aliases, needs an alias.h file -->
  <xsl:if test="($htmlhelp.force.map.and.alias != 0) or
		(//processing-instruction('dbhh')) or
		(key('id','')//processing-instruction('dbhh'))">
		<xsl:text>
[ALIAS]
#include </xsl:text>
  <xsl:value-of select="$htmlhelp.alias.file"/>

  <!-- Section5: MAP, needs a context.h file -->
  <xsl:text>

[MAP]
#include </xsl:text>
  <xsl:value-of select="$htmlhelp.map.file"/><xsl:text>
</xsl:text>
  </xsl:if>

  <xsl:value-of select="$htmlhelp.hhp.tail"/>
</xsl:template>


<!--!!====================================================================
ConvertDecToHex
:Revision:version="0.1" date="2018-06-27" status="draft"

Convert any number from decimal to hexadecimal format
$index: dec number to convert to hexadecimal format
-->

<xsl:template name="ConvertDecToHex">
  <xsl:param name="index" />

  <xsl:if test="$index > 0">
    <xsl:call-template name="ConvertDecToHex">
      <xsl:with-param name="index" select="floor($index div 16)" />
    </xsl:call-template>
    <xsl:choose>
      <xsl:when test="$index mod 16 &lt; 10">
        <xsl:value-of select="$index mod 16" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="$index mod 16 = 10">A</xsl:when>
          <xsl:when test="$index mod 16 = 11">B</xsl:when>
          <xsl:when test="$index mod 16 = 12">C</xsl:when>
          <xsl:when test="$index mod 16 = 13">D</xsl:when>
          <xsl:when test="$index mod 16 = 14">E</xsl:when>
          <xsl:when test="$index mod 16 = 15">F</xsl:when>
          <xsl:otherwise>A</xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>


<!--!!====================================================================
hhc
Creates the TOC file
:Revision:version="0.2" date="2018-08-07" status="draft"

This template creates the table of contents file (.HHC)
-->

<xsl:template name="hhc">
  <xsl:call-template name="write.chunk">
    <xsl:with-param name="filename">
      <xsl:value-of select="$htmlhelp.hhc"/>
    </xsl:with-param>
    <xsl:with-param name="indent" select="'no'"/>
    <xsl:with-param name="content">
      <xsl:call-template name="hhc-main"/>
    </xsl:with-param>
    <xsl:with-param name="encoding">
      <xsl:call-template name="set-encoding" />
      </xsl:with-param>
    <xsl:with-param name="quiet" select="$chunk.quietly"/>
  </xsl:call-template>
</xsl:template>


<!--!!====================================================================
hhc-main
Creates the contents of the TOC file
:Revision:version="0.3" date="2018-08-07" status="draft"

This template generates the contents of TOC file (.HHC)
-->

<xsl:template name="hhc-main">
<HTML>&lf;
 <HEAD></HEAD>&lf;
 <BODY>&lf;
  <xsl:if test="$htmlhelp.hhc.folders.instead.books != 0">
   <OBJECT type="text/site properties">&lf;
     <param name="ImageType" value="Folder"/>&lf;
   </OBJECT>&lf;
  </xsl:if>

  <xsl:variable name="content">
    <xsl:apply-templates select="/" mode="hhc"/>
  </xsl:variable>

  <xsl:choose>
    <!-- there be an entry for the root element in the ToC -->
    <xsl:when test="$htmlhelp.hhc.show.root != 0">
      <UL>&lf;
      <LI><OBJECT type="text/sitemap">&lf;
	<param name="Name">
	  <xsl:attribute name="value">
	    <xsl:for-each select="key('page-type','guide')">
	      <xsl:value-of select="mal:title[1]/text()[last()]" />
	    </xsl:for-each>
	  </xsl:attribute>
	</param>&lf;
	<param name="Local">
	  <xsl:attribute name="value" >
	    <xsl:for-each select="key('page-type','guide')">
	      <xsl:value-of select="concat(@id[1],$html.extension)" />
	    </xsl:for-each>
	  </xsl:attribute>
	</param>
	</OBJECT></LI>&lf;
        <xsl:copy-of select="$content"/>
      </UL>&lf;
    </xsl:when>
    <xsl:otherwise>
      <!--no entry for the root element in the ToC -->
    <xsl:copy-of select="$content"/>
    </xsl:otherwise>
  </xsl:choose>

 </BODY>
</HTML>
</xsl:template> <!-- hhc-main -->


<!--!!====================================================================
hhc-entry
Generates a generic entry (TOC)
:Revision:version="0.4" date="2018-08-07" status="draft"

This template generates a list <LI> item in an unordered list <UL> of the
toc file (.HHC)
-->

<xsl:template name="hhc.entry">
  <xsl:param name="title" />
  <xsl:param name="link" />

  <LI><OBJECT type="text/sitemap">&lf;
    <param name="Name">
      <xsl:attribute name="value">
          <xsl:value-of select="normalize-space($title)"/>
      </xsl:attribute>
    </param>&lf;
    <param name="Local">
      <xsl:attribute name="value">
	<!--          <xsl:call-template name="href.target.with.base.dir"/> -->
	<xsl:value-of select="normalize-space($link)"/>
      </xsl:attribute>
    </param>
  </OBJECT></LI>&lf;
</xsl:template><!-- hhc.entry -->


<!--!!====================================================================
hhc templates
:Revision:version="0.8" date="2018-08-07" status="draft"
-->

  <xsl:template match="/" mode="hhc">
    <UL>&lf;
    <xsl:for-each select="key('page-id','index')">
        <xsl:sort select="../../mal:title"/>
	<xsl:call-template name="hhc.entry">
	  <xsl:with-param name="title" select="../../mal:title" />
	  <xsl:with-param name="link" select="concat(../../@id,$html.extension)" />
	</xsl:call-template>
    </xsl:for-each>
    </UL>&lf;

    <!-- sections -->
    <xsl:for-each select="key('page-type','guide')">
      <xsl:apply-templates select="mal:section" mode="hhc"/>
    </xsl:for-each>
  </xsl:template>


<!-- sub-templates - Level #1 -->

<xsl:template match="mal:section" mode="hhc">
    <UL>&lf;
    <LI><OBJECT type="text/sitemap">
      <param name="Name">
	<xsl:attribute name="value">
	  <xsl:value-of select="mal:title" />
	</xsl:attribute>
      </param>
        </OBJECT></LI>&lf;
	<xsl:variable name="my_id" select="@id" />
<UL>&lf;
        <xsl:for-each select="key('page-id',$my_id)">
	  <xsl:sort select="../../mal:title"/>
	  <xsl:call-template name="hhc.entry">
	    <xsl:with-param name="title" select="../../mal:title" />
	    <xsl:with-param name="link" select="concat(../../@id,$html.extension)" />
	  </xsl:call-template>
	</xsl:for-each>
</UL>&lf;
</UL>&lf;

</xsl:template>


<!--!-!===============================================================
write.chunk
:Revision:version="0.2" date="2018-08-07" status="draft"

Output to a file
-->

<xsl:template name="write.chunk">
  <xsl:param name="filename" select="''"/>
  <xsl:param name="quiet" select="$chunk.quietly"/>
  <xsl:param name="suppress-context-node-name" select="0"/>
  <xsl:param name="message-prolog"/>
  <xsl:param name="message-epilog"/>
  <xsl:param name="method" select="$chunker.output.method"/>
  <xsl:param name="encoding" select="$chunker.output.encoding"/>
  <xsl:param name="indent" select="$chunker.output.indent"/>
  <xsl:param name="omit-xml-declaration"
             select="$chunker.output.omit-xml-declaration"/>
  <xsl:param name="standalone" select="$chunker.output.standalone"/>
  <xsl:param name="doctype-public" select="$chunker.output.doctype-public"/>
  <xsl:param name="doctype-system" select="$chunker.output.doctype-system"/>
  <xsl:param name="media-type" select="$chunker.output.media-type"/>
  <xsl:param name="cdata-section-elements"
             select="$chunker.output.cdata-section-elements"/>
  <xsl:param name="content"/>

  <xsl:if test="$quiet = 0">
    <xsl:message>
      <xsl:if test="not($message-prolog = '')">
        <xsl:value-of select="$message-prolog"/>
      </xsl:if>
      <xsl:text>Writing </xsl:text>
      <xsl:value-of select="$filename"/>
      <xsl:if test="not($message-epilog = '')">
        <xsl:value-of select="$message-epilog"/>
      </xsl:if>
    </xsl:message>
  </xsl:if>

  <xsl:choose>
    <xsl:when test="element-available('exsl:document')">
      <xsl:choose>
        <!-- Handle the permutations ... -->
        <xsl:when test="$media-type != ''">
          <xsl:choose>
            <xsl:when test="$doctype-public != '' and $doctype-system != ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             media-type="{$media-type}"
                             doctype-public="{$doctype-public}"
                             doctype-system="{$doctype-system}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:when test="$doctype-public != '' and $doctype-system = ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             media-type="{$media-type}"
                             doctype-public="{$doctype-public}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:when test="$doctype-public = '' and $doctype-system != ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             media-type="{$media-type}"
                             doctype-system="{$doctype-system}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:otherwise><!-- $doctype-public = '' and $doctype-system = ''"> -->
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             media-type="{$media-type}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when test="$doctype-public != '' and $doctype-system != ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             doctype-public="{$doctype-public}"
                             doctype-system="{$doctype-system}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:when test="$doctype-public != '' and $doctype-system = ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             doctype-public="{$doctype-public}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:when test="$doctype-public = '' and $doctype-system != ''">
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             doctype-system="{$doctype-system}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:when>
            <xsl:otherwise><!-- $doctype-public = '' and $doctype-system = ''"> -->
              <exsl:document href="{$filename}"
                             method="{$method}"
                             encoding="{$encoding}"
                             indent="{$indent}"
                             omit-xml-declaration="{$omit-xml-declaration}"
                             cdata-section-elements="{$cdata-section-elements}"
                             standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </exsl:document>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>

    <xsl:when test="element-available('saxon:output')">
      <xsl:choose>
        <!-- Handle the permutations ... -->
        <xsl:when test="$media-type != ''">
          <xsl:choose>
            <xsl:when test="$doctype-public != '' and $doctype-system != ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            media-type="{$media-type}"
                            doctype-public="{$doctype-public}"
                            doctype-system="{$doctype-system}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:when test="$doctype-public != '' and $doctype-system = ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            media-type="{$media-type}"
                            doctype-public="{$doctype-public}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:when test="$doctype-public = '' and $doctype-system != ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            media-type="{$media-type}"
                            doctype-system="{$doctype-system}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:otherwise><!-- $doctype-public = '' and $doctype-system = ''"> -->
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            media-type="{$media-type}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when test="$doctype-public != '' and $doctype-system != ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            doctype-public="{$doctype-public}"
                            doctype-system="{$doctype-system}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:when test="$doctype-public != '' and $doctype-system = ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            doctype-public="{$doctype-public}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:when test="$doctype-public = '' and $doctype-system != ''">
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            doctype-system="{$doctype-system}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:when>
            <xsl:otherwise><!-- $doctype-public = '' and $doctype-system = ''"> -->
              <saxon:output saxon:character-representation="{$saxon.character.representation}"
                            href="{$filename}"
                            method="{$method}"
                            encoding="{$encoding}"
                            indent="{$indent}"
                            omit-xml-declaration="{$omit-xml-declaration}"
                            cdata-section-elements="{$cdata-section-elements}"
                            standalone="{$standalone}">
                <xsl:copy-of select="$content"/>
              </saxon:output>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>

    <xsl:when test="element-available('redirect:write')">
      <!-- Xalan uses redirect -->
      <redirect:write file="{$filename}">
        <xsl:copy-of select="$content"/>
      </redirect:write>
    </xsl:when>

    <xsl:otherwise>
      <!-- it doesn't matter since we won't be making chunks... -->
      <xsl:message terminate="yes">
        <xsl:text>Can't make chunks with </xsl:text>
        <xsl:value-of select="system-property('xsl:vendor')"/>
        <xsl:text>'s processor.</xsl:text>
      </xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--!!==========================================================================
set-langID
Set the Language property in the htmlhelp.hhp project file
:Revision:version="0.1" date="2018-08-07" status="draft"

The Help compiler needs to know what language the project files are in, because
they do not have any encoding identifiers like the HTML files do. That
information is supplied by a Language property in the htmlhelp.hhp project file.

That property is inserted into the project file by the stylesheet based on the
root element's lang attribute in your DocBook document. If there is no lang
attribute on the root element, then en is used.
-->

<xsl:template name="set-langID">
  <xsl:choose>
    <xsl:when test="$htmlhelp.langID = ''">
      <xsl:choose>
	<!-- Arabic -->
	<xsl:when test="//mal:page[lang('ar')]">
	  <xsl:text>0x1000 Arabic</xsl:text>
	</xsl:when>
	<!-- Farsi -->
	<xsl:when test="//mal:page[lang('fa')]">
	  <xsl:text>0x0429 Farsi</xsl:text>
	</xsl:when>
	<!-- French -->
	<xsl:when test="//mal:page[lang('fr')]">
	  <xsl:text>0x040c French</xsl:text>
	</xsl:when>
	<!-- German -->
	<xsl:when test="//mal:page[lang('de')]">
	  <xsl:text>0x0407 German</xsl:text>
	</xsl:when>
	<!-- Spanish -->
	<xsl:when test="//mal:page[lang('es')]">
	  <xsl:text>0x040a Spanish (Traditional Sort)</xsl:text>
	</xsl:when>
	<!-- Hebrew -->
	<xsl:when test="//mal:page[lang('he')]">
	  <xsl:text>0x040d Hebrew</xsl:text>
	</xsl:when>
	<!-- Portuguese -->
	<xsl:when test="//mal:page[lang('pt')]">
	  <xsl:text>0x0416 Portuguese</xsl:text>
	</xsl:when>
	<!-- Russian -->
	<xsl:when test="//mal:page[lang('ru')]">
	  <xsl:text>0x0419 Russian</xsl:text>
	</xsl:when>
	<!-- Chinese -->
	<xsl:when test="//mal:page[lang('zh')]">
	  <xsl:text>0x0804 Chinese</xsl:text>
	</xsl:when>
	<!-- set lang to default (English) -->
      <xsl:otherwise>
	<xsl:text>0x0409 English (United States)</xsl:text>
      </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <!-- set lang to custom selection -->
      <xsl:value-of select="$htmlhelp.langID"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!--!!==========================================================================
set-encoding
Set the encoding of the HTML output (TOC only) (.HHC)
:Revision:version="0.1" date="2018-08-07" status="draft"

The encoding of the HTML output has to be one that the Help compiler recognizes.
Unfortunately, the Microsoft Help compiler does not recognize UTF-8 encoding.
Two encodings that are often used are iso-8859-1 and windows-1252.

You can establish the output encoding of the stylesheet using the
htmlhelp.encoding parameter. Default is set to iso-8859-1 by default. That
encoding covers the basic European languages, but does not contain some special
characters such as longer dashes, typographical quotes, or the ™ or Euro
symbols. The windows-1252 encoding is identical to iso-8859-1 over most of its
range, but includes more special characters, including trademark and euro.

If you want to use windows-1252 as your output encoding, you have to consider
what XSLT processor you are using. The xsltproc processor can output
windows-1252, as can any XSLT processor that implements the EXSLT document()
extension function.
-->

<xsl:template name="set-encoding">
  <xsl:choose>
    <xsl:when test="$htmlhelp.encoding = ''">
      <xsl:choose>
	<!-- Arabic encoding -->
	<xsl:when test="//mal:page[lang('ar')]">
	  <xsl:text>windows-1256</xsl:text>
	</xsl:when>
	<!-- Farsi encoding -->
	<xsl:when test="//mal:page[lang('fa')]">
	  <xsl:text>windows-1256</xsl:text>
	</xsl:when>
	<!-- French encoding -->
	<xsl:when test="//mal:page[lang('fr')]">
	  <xsl:text>windows-1252</xsl:text>
	</xsl:when>
	<!-- German encoding -->
	<xsl:when test="//mal:page[lang('de')]">
	  <xsl:text>windows-1252</xsl:text>
	</xsl:when>
	<!-- Spanish encoding -->
	<xsl:when test="//mal:page[lang('es')]">
	  <xsl:text>windows-1252</xsl:text>
	</xsl:when>
	<!-- Hebrew encoding -->
	<xsl:when test="//mal:page[lang('he')]">
	  <xsl:text>windows-1255</xsl:text>
	</xsl:when>
	<!-- Portuguese encoding -->
	<xsl:when test="//mal:page[lang('pt')]">
	  <xsl:text>windows-1252</xsl:text>
	</xsl:when>
	<!-- Russian encoding -->
	<xsl:when test="//mal:page[lang('ru')]">
	  <xsl:text>windows-1251</xsl:text>
	</xsl:when>
	<!-- Chinese encoding -->
	<xsl:when test="//mal:page[lang('zh')]">
	  <xsl:text>GB18030</xsl:text>
	</xsl:when>
	<!-- set default encoding (English) -->
	<xsl:otherwise>
	  <xsl:text>iso-8859-1</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <!-- set to a custom value -->
      <xsl:value-of select="$htmlhelp.encoding"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


</xsl:stylesheet>
