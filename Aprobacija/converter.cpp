/*
Copyright (C) 2021 Aleksandr Migunov

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "converter.h"
#include <QtCore>
#include <QRegExp>

QString converter_to_html (QString str){
    str.replace("<", "##lt;");
    str.replace(">", "##gt;");
    str.replace("\\", "&&");

    str.replace("&&ide", "<br>");  //ide
    str.replace("&&id", "<p>");  //id
    str.replace("&&sts", "<br>");  //sts
    str.replace("&&ili1", "<li>"); //ili1
    str.replace("&&ili", "<li>"); //ili
    str.replace("&&pi1", "<p>"); //pi1
    str.replace("&&pi", "<p>"); //pi
    str.replace("&&ie", "<br>"); //ie
    str.replace("&&iot", "<h3>"); //iot
    str.replace("&&ior*", ""); //ior*
    str.replace("&&ior ", "");
    str.replace("&&ior", ""); //ior
    str.replace("&&io1", "<br><li>"); //io1
    str.replace("&&io2", "<br><li>"); //io2
    str.replace("&&io", "<br><li>"); //io
    str.replace("&&ib", "<br>"); //ib
    str.replace("&&periph", "<p>"); //periph
    str.replace("&&it*", "</i>"); //it*
    str.replace("&&it", "<i>"); //it
    str.replace("&&pmr", "<p>"); //pmr
    str.replace("&&rem", "<p>"); //rem
    str.replace("&&fig*", ""); //fig*
    str.replace("&&fig", ""); //fig
    str.replace("&&w*", ""); //w*
    str.replace("&&w", ""); //w !!
    str.replace("&&bk*", "</i>"); //bk*
    str.replace("&&bk", "<i>"); //bk
    str.replace("&&bd*", "</i></b>"); //bd*
    str.replace("&&bd", "<b><i>"); //bd
    str.replace("&&fl ", "");
    str.replace("&&fl", ""); //fl
    str.replace("&&fm*",""); //fm*
    str.replace("&&fm ","");
    str.replace("&&fm",""); //fm
    str.replace("&&nb", "<br>"); //nb
    str.replace("&&k*", "</i>"); //k*
    str.replace("&&k", "<i>"); //k !!
    str.replace("&&add*", "</i></b>"); //add*
    str.replace("&&add ", "<b><i>");
    str.replace("&&add", "<b><i>"); //add
    str.replace("&&sc*", "</font>"); //sc*
    str.replace("&&sc", "<font style = \"font-variant: small-caps\">"); //sc
    str.replace("&&nd*", "</b></font>"); //nd*
    str.replace("&&nd", "<font style = \"font-variant: small-caps\"><b>"); //nd
    str.replace("&&ord*", "</sup>"); //ord*
    str.replace("&&ord", "<sup>"); //ord
    str.replace("&&pmo", "<br>"); //pmo
    str.replace("&&pm", "<br>"); //pm

    //poetry
    str.replace("&&qs*", "</i>"); //qs*
    str.replace("&&qs ", "<br><i>");
    str.replace("&&qs", "<br><i>"); //qs
    str.replace("&&qr", "<br><i>"); //qr
    str.replace("&&qm1", "<br>"); //qm1
    str.replace("&&qm2", "<br>"); //qm2
    str.replace("&&q1", "</i><br>"); //q1 //correction after \d in PBR Psalms
    str.replace("&&q2", "<br>"); //q2
    str.replace("&&q3", "<br>"); //q3
    str.replace("&&qc", "<p>"); //qc
    str.replace("&&q", "<br>"); //q !!

    //lists
    str.replace("&&li1", "<li>"); //li1
    str.replace("&&li2", "<li>"); //li2
    str.replace("&&li3", "<li>"); //li3
    str.replace("&&li4", "<li>"); //li4
    str.replace("&&li", "<li>"); //li
    str.replace("&&mi", "<li>"); //mi

    //book subtitles
    str.replace("&&toc1", "<p><i>"); //toc1
    str.replace("&&toc2", "<p><i>"); //toc2
    str.replace("&&toc3", "<p><i>"); //toc3

    //book titles
    str.replace("&&mt4", "<h3>"); //mt4
    str.replace("&&mt1", "<h1>"); //mt1
    str.replace("&&mt2", "<h1>"); //mt2
    str.replace("&&mt3", "<h1>"); //mt3
    str.replace("&&mt", "<h1>"); //mt
    str.replace("&&h", "<h1>"); //h !!

    //introduction
    str.replace("&&imt", "<h2>"); //imt
    str.replace("&&im", "<p>"); //im
    str.replace("&&ipr", "<p>"); //ipr
    str.replace("&&ip", "<p>"); //ip

    //chapter titles
    str.replace("&&cl", "<h2>"); //cl
    str.replace("&&cp", "<h2>"); //cp

    //chapters
    str.replace("&&c", "<p><b>"); //c !!

    //sections
    str.replace("&&sr", "<b><i>"); //sr
    str.replace("&&sp", "<p><i>"); //sp

    //section titles
    str.replace("&&is1", "<h3>"); //is1
    str.replace("&&is2", "<h3>"); //is2
    str.replace("&&ms1", "<h2>"); //ms1
    str.replace("&&ms", "<h2>"); //ms
    str.replace("&&s1", "<h3>"); //s1
    str.replace("&&s2", "<h3>"); //s2
    str.replace("&&s", "<h2>"); //s !!
    str.replace("&&d\n&&v", "<p><i><sup>"); //d v
    str.replace("&&d &&v", "<p><i><sup>");
    str.replace("&&d&&v", "<p><i><sup>");

    //section references
    str.replace("&&mr", "<b><i>"); //mr
    str.replace("&&r", "<b><i>"); //r !!
    str.replace("&&d", "<p><i>"); //d !!

    //verses
    str.replace("&&fv", "<sup>"); //fv
    str.replace("&&v", "<sup>"); //v !!

    str.replace("&&+tl*", "</i>"); //+tl*
    str.replace("&&+tl ", "<i>");
    str.replace("&&+tl", "<i>"); //+tl
    str.replace("&&tl*", "</i>"); //tl*
    str.replace("&&tl ", "<i>");
    str.replace("&&tl", "<i>"); //tl
    str.replace("&&fq", "<i>"); //fq
    str.replace("&&+add*", "</i></b>"); //+add*
    str.replace("&&+add ", "<b><i>");
    str.replace("&&+add", "<b><i>"); //+add

    //footnotes
    str.replace("&&f + &&fk ", " &&["); //f + fk //correction
    str.replace("&&f + &&fr ", " &&["); //f + fr //correction
    str.replace("&&f - &&fr ", " &&["); //f - fr //correction
    str.replace("&&ft*", ""); //ft*
    str.replace("&&fk*", ""); //fk*
    str.replace("&&ft", "&&&"); //ft  //corrrection
    str.replace("&&fk", "<b><i>"); //fk
    str.replace("&&fr*", ""); //fr*
    str.replace("&&f*", "]"); //f*
    str.replace("&&f + ", " ["); //f +

    //verses footnotes
    str.replace("&&+xt*", ""); //+xt*
    str.replace("&&+xt ", "");
    str.replace("&&+xt", ""); //+xt
    str.replace("&&xt*", ""); //xt*
    str.replace("&&xt ", "");
    str.replace("&&xt", ""); //xt
    str.replace("&&x - &&xo ", "&&[["); //x - xo //correction
    str.replace("&&x -  &&xo ", "&&[["); //x -  xo //correction
    str.replace(" &&x*", "]] ");
    str.replace("&&x*", "]] "); //x*
    str.replace(" &&xo", "");
    str.replace("&&xo", ""); //xo

    //table
    str.replace("&&tr", "<tr>"); //tr
    str.replace("&&thc1", "<th>"); //thc1
    str.replace("&&thc2", "<th>"); //thc2
    str.replace("&&thc3", "<th>"); //thc3
    str.replace("&&thc4", "<th>"); //thc4
    str.replace("&&tc1", "<td>"); //tc1
    str.replace("&&tc2", "<td>"); //tc2
    str.replace("&&tc3", "<td>"); //tc3
    str.replace("&&tc4", "<td>"); //tc4
    str.replace("&&tcr1", "<td>"); //tcr1
    str.replace("&&tcr2", "<td>"); //tcr2
    str.replace("&&tcr3", "<td>"); //tcr3
    str.replace("&&tcr4", "<td>"); //tcr4

    str.replace("&&b", "<br>"); //b !!
    str.replace("&&m", "<br>"); //m !!
    str.replace("&&pc", "<p>"); //pc
    str.replace("&&p", "<p>"); //p !!

    //adding closing tags
    QRegExp rgx1("<h1> ([^&<]+)");
    str.replace(rgx1, "<h1>\\1</h1>");
    QRegExp rgx2("<h2> ([^&<]+)");
    str.replace(rgx2, "<h2>\\1</h2>");
    QRegExp rgx3("<h3> ([^&<]+)");
    str.replace(rgx3, "<h3>\\1</h3>");
    QRegExp rgx4("<p><b> ([0-9]{1,3})");
    str.replace(rgx4, "<p><b>\\1</b><p>");
    QRegExp rgx5("<p><i><sup> ([0-9]{1,3}) ([^&<]+)");
    str.replace(rgx5, "<p><i><sup>\\1</sup>\\2</i>");
    QRegExp rgx6("<sup> ([0-9]{1,3}-[0-9]{1,3})");
    str.replace(rgx6, "<sup>\\1</sup>");
    QRegExp rgx7("<sup> ([0-9]{1,3})");
    str.replace(rgx7, "<sup>\\1</sup>");
    QRegExp rgx8("<p><b><i> ([^&<]+)");
    str.replace(rgx8, "<p><b><i>\\1</i></b>");
    QRegExp rgx9("<b><i> ([^&<]+)");
    str.replace(rgx9, "<b><i>\\1</i></b>");
    QRegExp rg10("<p><i> ([^&<]+)");
    str.replace(rg10, "<p><i>\\1</i>");
    QRegExp rgx11("<b><i> ([^&<]+)");
    str.replace(rgx11, "<b><i>\\1</i></b>");
    QRegExp rgx12("<i> ([^&<]+)");
    str.replace(rgx12, "<i>\\1</i>");
    QRegExp rgx13("<td> ([^&<]+)<p><b>");
    str.replace(rgx13, "<td>\\1</td></tr></table><p><b>");
    QRegExp rgx14("<td> ([^&<]+)<br>");
    str.replace(rgx14, "<td>\\1</td></tr></table><br>");
    QRegExp rgx15("<td> ([^&<]+)<h1>");
    str.replace(rgx15, "<td>\\1</td></tr></table><h1>");
    QRegExp rgx16("<td> ([^&<]+)<h2>");
    str.replace(rgx16, "<td>\\1</td></tr></table><h2>");
    QRegExp rgx17("<td> ([^&<]+)<h3>");
    str.replace(rgx17, "<td>\\1</td></tr></table><h3>");
    QRegExp rgx18("<td> ([^&<]+)<h4>");
    str.replace(rgx18, "<td>\\1</td></tr></table><h4>");
    QRegExp rgx19("<td> ([^&<]+)<p>");
    str.replace(rgx19, "<td>\\1</td><p>");

    str.replace("&&&", ""); //correction
    str.replace("&&[", "["); //correction
    str.replace("##lt;", "&lt;");
    str.replace("##gt;", "&gt;");

    return str;
}

QString converter_from_transcelerator(QString str) {
    str.replace("\n", "%%"); //correction for HTML closing tags

    str.replace("\\_sh", "<p>sh");
    str.replace("\\rf", "<h1>");
    QRegExp rgx20("<h1> ([^%]+)");
    str.replace(rgx20, "<h1>\\1</h1>");
    str.replace("\\oht", "<h3>");
    str.replace("\\oh", "<h3>");
    str.replace("\\dh", "<h3>");
    QRegExp rgx21("<h3> ([^%]+)");
    str.replace(rgx21, "<h3>\\1</h3>");
    str.replace("\\vt", "<br><b>");
    str.replace("\\cmt", "<br>");
    str.replace("\\tqref", "<br><b>");
    str.replace("\\tqrel", "<br>");
    str.replace("\\tqe", "<br>");
    str.replace("\\tq", "<br><b>");
    str.replace("\\bttq", "<br><b>");
    QRegExp rgx22("<b> ([^%]+)");
    str.replace(rgx22, "<b>\\1</b>");
    str.replace("\\an", "<br><i>");
    QRegExp rgx23("<i> ([^%]+)");
    str.replace(rgx23, "<i>\\1</i>");

    str.replace("%%", "\n"); //correction to restore spaces

    return str;
}
