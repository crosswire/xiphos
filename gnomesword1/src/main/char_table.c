/* 
 * added to gnomesword 2001-01-06  for spell checking
*/

/* Bluefish HTML Editor
 * char_table.h - character convertion prototypes
 *
 * Copyright (C) 2000 Olivier Sessink & Pablo De Napoli (for this module)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>
#include "main/char_table.h"
#include "main/debug.h"


Tchar_entity ascii_charset[] = {
	{34, "&quot;"}, {38, "&amp;"}, {60, "&lt;"}, {62, "&gt;"}, {0, NULL}
};

/* Do not modify this table */
/* For convert iso to html to work properly */

Tchar_entity iso8859_1_charset[] = {
   	 {160, "&nbsp;"}, {161, "&iexcl;"}, {162, "&cent;"}, {163, "&pound;"},
		{164, "&curren;"}, {165, "&yen;"}, {166, "&brvbar;"}, {167, "&sect;"},
		{168, "&uml;"}, {169, "&copy;"}, {170, "&ordf;"}, {171, "&laquo;"},
		{172, "&not;"}, {173, "&shy;"}, {174, "&reg;"}, {175, "&macr;"},
		{176, "&deg;"}, {177, "&plusmn;"}, {178, "&sup2;"}, {179, "&sup3;"},
		{180, "&acute;"}, {181, "&micro;"}, {182, "&para;"}, {183, "&middot;"},
		{184, "&cedil;"}, {185, "&sup1;"}, {186, "&ordm;"}, {187,"&raquo;"},
		{188, "&frac14;"}, {189, "&frac12;"}, {190, "&frac34;"}, {191,"&iquest;"},
		{192, "&Agrave;"}, {193, "&Aacute;"}, {194, "&Acirc;"}, {195, "&Atilde;"},
		{196, "&Auml;"}, {197, "&Aring;"}, {198, "&AElig;"}, {199,  "&Ccedil;"},
		{200, "&Egrave;"}, {201, "&Eacute;"}, {202, "&Ecirc;"}, {203,
																 "&Euml;"},
		{204, "&Igrave;"}, {205, "&Iacute;"}, {206, "&Icirc;"}, {207,
																 "&Iuml;"},
		{208, "&ETH;"}, {209, "&Ntilde;"}, {210, "&Ograve;"}, {211,
															   "&Oacute;"},
		{212, "&Ocirc;"}, {213, "&Otilde;"}, {214, "&Ouml;"}, {215,
															   "&times;"},
		{216, "&Oslash;"}, {217, "&Ugrave;"}, {218, "&Uacute;"}, {219,
																  "&Ucirc;"},
		{220, "&Uuml;"}, {221, "&Yacute;"}, {222, "&THORN;"}, {223,
															   "&szlig;"},
		{224, "&agrave;"}, {225, "&aacute;"}, {226, "&acirc;"}, {227,
																 "&atilde;"},
		{228, "&auml;"}, {229, "&aring;"}, {230, "&aelig;"}, {231,
															  "&ccedil;"},
		{232, "&egrave;"}, {233, "&eacute;"}, {234, "&ecirc;"}, {235,
																 "&euml;"},
		{236, "&igrave;"}, {237, "&iacute;"}, {238, "&icirc;"}, {239,
																 "&iuml;"},
		{240, "&eth;"}, {241, "&ntilde;"}, {242, "&ograve;"}, {243,
															   "&oacute;"},
		{244, "&ocirc;"}, {245, "&otilde;"}, {246, "&ouml;"}, {247,
															   "&divide;"},
		{248, "&oslash;"}, {249, "&ugrave;"}, {250, "&uacute;"}, {251,
																  "&ucirc;"},
		{252, "&uuml;"}, {253, "&yacute;"}, {254, "&thorn;"}, {255,
															   "&yuml;"}
	, {0, NULL}
};

gboolean isalpha_iso(unsigned char c)
/* test for iso-8859-1 alphabetical characters  */
{
 /* Fixme: is character 223 non alpha ? */
 DEBUG_MSG("Testing if %c(code %i) is alpha iso \n",c,c);
 return((c>=192) && (c!=215) && (c!=222) && (c!=223) && (c!=247) &&(c!=254));
}

/* Convert a speciall character from html to iso_8859_1 or ascii */
/* If charset = ANY_CHAR_SET look up in both tables */
/* If convertion fails, returns '\0' */

gchar convert_from_html_chars (char* character ,Tchar_entity charset[])
{
 gint j;
 gchar c;
 DEBUG_MSG("running convert_form_html_chars\n");
 DEBUG_MSG("character=\" %s \" \n",character);
 j=0;
 if (charset==ANY_CHAR_SET)
   {
     c = convert_from_html_chars(character,iso8859_1_charset);
     if (c !='\0')
       return c;
     else
       return(convert_from_html_chars(character,ascii_charset));
   }
 else
 {
 DEBUG_MSG("Character to convert= \" %s \" \n",character);
 while (charset[j].entity != NULL)
 {
   if (strcmp(charset[j].entity,character)==0)
     {
      DEBUG_MSG("match \"%s\"\n",charset[j].entity);
      DEBUG_MSG("converted character='%c' \n", charset[j].id);
      return (charset[j].id);
     }
   else
      j++;
 }
 return ('\0'); /* if could not be converted */
 }
}

gchar* convert_char_iso_to_html (unsigned char c)
/*  also converts ascii chars */
{
 gint i;
 DEBUG_MSG("Converting iso char '%c' to html \n",c);
 if (c>=160)
   return(iso8859_1_charset[c-160].entity);
 else
   for (i=0;i<3;i++)
      if (ascii_charset[i].id==c)
        return(ascii_charset[i].entity);
   DEBUG_MSG("Unconverted\n");
   return(NULL); /* if cannot be converted */
}

gchar* convert_string_iso_to_html (gchar* string)
{
 gchar* converted_string;
 gchar* converted_char;
 gchar* p;
 DEBUG_MSG("Converting string \"%s\" from iso to html\n",string);
 converted_string = g_malloc(8*strlen(string));
  /* for the converted string we need at most 8 times the original length
  This function is designed to save time , not memory */
  p = converted_string;
   while (*string !='\0')
   {
     converted_char = convert_char_iso_to_html(*string);
     if (converted_char==NULL)
       {
       *p = *string;
       p ++;
       }
     else
       {
       DEBUG_MSG("Converted char: %s\n",converted_char);
       while (*converted_char !='\0')
         {
          *p = * converted_char;
          p++;
          converted_char++;
         }
       };
     string++;
    }
   *p ='\0';
  DEBUG_MSG("Converted string:\"%s\" \n",converted_string);
  return(converted_string);
}


