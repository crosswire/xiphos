<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:html="http://www.w3.org/1999/xhtml"
                xmlns:exsl="http://exslt.org/common"
                xmlns:set="http://exslt.org/sets"
                xmlns="http://www.w3.org/1999/xhtml"
                exclude-result-prefixes="html set"
                extension-element-prefixes="exsl"
                version="1.0">

<!--**==========================================================================
html.page
Create an HTML document.
:Revision:version="1.0" date="2018-07-04" status="final"
$node: The node to create HTML for.

This template overrides the actual {html.page}. It outputs MS HTML compliant
elements and container divs, and calls various templates and modes
to output the inner content.
-->

<xsl:template name="html.page">
  <xsl:param name="node" select="."/>
  <html>
    <head>
      <meta name="viewport"
            content="width=device-width, initial-scale=1.0, user-scalable=yes"/>
      <title>
        <xsl:apply-templates mode="html.title.mode" select="$node"/>
      </title>
      <xsl:call-template name="html.css">
        <xsl:with-param name="node" select="$node"/>
      </xsl:call-template>
    </head>
    <body>
      <xsl:call-template name="html.lang.attrs">
        <xsl:with-param name="node" select="$node"/>
      </xsl:call-template>
      <xsl:apply-templates mode="html.body.attr.mode" select="$node"/>
      <xsl:call-template name="html.top.custom">
        <xsl:with-param name="node" select="$node"/>
      </xsl:call-template>
        <div class="page">
            <div class="inner pagewide">
              <xsl:apply-templates mode="html.header.mode" select="$node"/>
            </div>
            <xsl:apply-templates mode="html.body.mode" select="$node"/>
            <!-- no footer -->
            <div class="inner pagewide">
            </div>
        </div>
	<xsl:call-template name="html.bottom.custom">
          <xsl:with-param name="node" select="$node"/>
	</xsl:call-template>
    </body>
  </html>
</xsl:template>




<!--**==========================================================================
html.css.custom
Output overrided CSS for an HTML output page.
:Revision:version="1.0" date="2018-07-04" status="final"
-->

<xsl:param name="color.gray_background" select="'#f7f6f5'"/>
<xsl:param name="color.gray_border" select="'#ccc1c1'"/>
<xsl:param name="color.blue_background" select="'#f7f6f5'"/>
<xsl:param name="color.blue_border" select="'#ccc1c1'"/>
<xsl:param name="color.yellow_background" select="'#f7f6f5'"/>
<xsl:param name="color.yellow_border" select="'#ccc1c1'"/>
<xsl:param name="color.red_background" select="'#f7f6f5'"/>
<xsl:param name="color.red_border" select="'#ccc1c1'"/>

<xsl:param name="color.text" select="'DarkSlateGrey'"/>
<xsl:param name="color.text_light" select="'LightSlateGrey'"/>
<xsl:param name="color.link" select="'CornflowerBlue'"/>
<xsl:param name="color.link_visited" select="'CornflowerBlue'"/>


<xsl:template name="html.css.custom">
  <xsl:param name="node" select="."/>
  <xsl:param name="direction">
    <xsl:call-template name="l10n.direction"/>
  </xsl:param>
  <xsl:param name="left">
    <xsl:call-template name="l10n.align.start">
      <xsl:with-param name="direction" select="$direction"/>
    </xsl:call-template>
  </xsl:param>
  <xsl:param name="right">
    <xsl:call-template name="l10n.align.end">
      <xsl:with-param name="direction" select="$direction"/>
    </xsl:call-template>
  </xsl:param>
  <xsl:text>
body{
  font-size: 12px;
}
  </xsl:text>
</xsl:template>
</xsl:stylesheet>
