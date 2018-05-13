<?xml version="1.0" encoding="utf-8"?>
<!--
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License
along with this program; see the file COPYING.LGPL.  If not, see <http://www.gnu.org/licenses/>.
-->

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- This stylesheet will generate a TOC file for HTML Help -->

   <xsl:output
       method="html"
       version="4.0"
       indent="yes"
       omit-xml-declaration="yes"
       encoding="utf-8"
       standalone="yes" />

   <xsl:strip-space elements="*"/>

   <!-- html structure -->
   <xsl:template match="/">
     <HTML>
       <HEAD>
	 <meta name="GENERATOR" content="Microsoft® HTML Help Workshop 4.1"/>
	   <!-- Sitemap 1.0 -->
       </HEAD>
       <BODY>
	 <UL>
	   <xsl:apply-templates/>
	 </UL>
       </BODY>
     </HTML>
   </xsl:template>

   <!-- discard <title> -->
   <xsl:template match="title">
   </xsl:template>

   <!-- discard <p> -->
   <xsl:template match="p">
   </xsl:template>

   <!-- discard footer -->
   <xsl:template match="div[@class='footer']">
   </xsl:template>

   <!-- group by class="inner" -->
   <xsl:template match="div[@class='inner']">
     <UL><!-- 2 -->
       <xsl:apply-templates/>
     </UL><!-- 2 -->
   </xsl:template>

   <xsl:template match="h1">
     <LI>
       <OBJECT type="text/sitemap">
	 <param name="Name" value="{span[@class='title']}" />
	 <param name="Local" value="index.html" />
	 <!--  <xsl:apply-templates/> -->
       </OBJECT>
     </LI>
   </xsl:template>

   <xsl:template match="h2">
     <LI>
       <OBJECT type="text/sitemap">
	 <param name="Name" value="{span[@class='title']}" />
	<!-- <xsl:apply-templates/> -->
       </OBJECT>
     </LI>
   </xsl:template>

   <!-- get links -->
   <xsl:template match="a">
     <LI>
       <OBJECT type="text/sitemap">
	 <param name="Name" value="{span[@class='title']}" />
	 <param name="Local" value="{@href}" />
       <!-- <xsl:value-of select="span[@href]" /> -->
       <!-- <xsl:value-of select="span[@class='title']" /> -->
       </OBJECT>
     </LI>
   </xsl:template>


</xsl:stylesheet>
