<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:output encoding="UTF-8" method="html" omit-xml-declaration="yes" indent="yes"/>

    <xsl:include href="basic_prsm.xsl"/>

    <xsl:template match="prsm">
        <html>
            <head>
                <title>Protein-Spectrum-Match for Spectrum #<xsl:value-of select="ms/ms_header/id"/> -
                    Species #<xsl:value-of select="annotated_protein/species-id"/> 
                    for <xsl:value-of select="annotated_protein/sequence_name"/>
                </title>
                <script type="text/javascript" src="../sorttable.js"/>

                <script type="text/javascript">
                    var peaksCount = <xsl:value-of select="count(ms/peaks/peak)"/>;

                    function isPeakMatched(peakId) {
                        <xsl:apply-templates select="ms/peaks/peak/matched_ions/matched_ion" mode="peakMatch"/>
                        return false;
                    }

                    function showMatchedPeaks() {
                        for (i = 0; peaksCount>i; i++) {
                            document.getElementById('peak' + i).style.display  = isPeakMatched(i) ? "" : "none";
                        }
                    }

                    function showNotMatchedPeaks() {
                        for (i = 0; peaksCount>i; i++) {
                            document.getElementById('peak' + i).style.display  = isPeakMatched(i) ? "none" : "";
                        }
                    }

                    function showAllPeaks() {
                        for (i = 0; peaksCount>i; i++) {
                            document.getElementById('peak' + i).style.display  = "";
                        }
                    }

                    function showIonPeaks(ids) {
                        for (i = 0; peaksCount>i; i++) {
                            document.getElementById('peak' + i).style.display  = "none";
                            document.getElementById('peak' + i).style.background  =  "#FFFFFF";
                        }
                        var s = ids.split(",");
                        for (i = 0; s.length>i; i++) {
                            
                            document.getElementById('peak' + s[i]).style.display  =  "";
                            document.getElementById('peak' + s[i]).style.background  =  "yellow";
                        }
                    }

                </script>
                <style>
                    td.sortableHeader{text-decoration:underline;}
                    @font-face {
                    font-family: "FreeMono";
                    src: url("../FreeMono.ttf")
                    }
                </style>
                <style type="text/css">
                    .outer {
                    position:relative;
                    padding-top:5em;
                    margin:0;
                    }

                    .innera {
                    overflow:auto;
                    width:52em;
                    height:28em;
                    margin:0;
                    }

                    .outer thead tr {
                    position:absolute;
                    top:1.5em;
                    height:1.5em;
                    left:0;
                    }

                    .outer th, .outer td {
                    text-align:left;
                    }
                    .info {
                    font-size:15 px
                    }
                </style>
            </head>
            <body>
                <xsl:call-template name="navigation"/>

                <h2>Protein-Spectrum-Match for Spectrum #<xsl:value-of select="ms/ms_header/id"/>
                </h2>

                <table cellpadding="3" width="750" class="info">
                    <tr>
                        <td>PrSM ID: </td>
                        <td>
                            <xsl:value-of select="prsm_id"/>
                        </td>
                        <td>
                            Scan(s): 
                        </td>
                        <td> 
                            <xsl:value-of select="ms/ms_header/scans"/>
                        </td>
                        <td>Precursor charge:</td>
                        <td>
                            <xsl:value-of select="ms/ms_header/precursor_charge"/>
                        </td>
                        <td>
                             Precursor m/z:
                        </td>
                        <td>
                            <xsl:value-of select="ms/ms_header/precursor_mz"/>
                        </td>
                    </tr>
                    <tr>
                        <td>Precursor mass:</td>
                        <td>
                            <xsl:value-of select="ms/ms_header/precursor_mass"/>
                        </td>
                        <td>Adjusted precursor mass:</td>
                        <td>
                            <xsl:value-of select="adjusted_precursor_mass"/>
                        </td>
                        <td># matched peaks:</td>
                        <td>
                            <xsl:value-of select="matched_peak_number"/>
                        </td>
                        <td># matched fragments:</td>
                        <td>
                            <xsl:value-of select="matched_fragment_number"/>
                        </td>
                    </tr>
                    <tr>
                        <td>E-value:</td>
                        <td>
                            <xsl:value-of select="e_value"/>
                        </td>
                        <td>P-value:</td>
                        <td>
                            <xsl:value-of select="p_value"/>
                        </td>
                        <td>Spectral FDR:</td>
                        <td>
                            <xsl:value-of select="fdr"/>
                        </td>
                        <td>Protein mass:</td>
                        <td>
                            <xsl:value-of select="annotated_protein/protein_mass"/>
                        </td>

                    </tr>
                    <tr>
                    </tr>
                </table>
                <br/>
                <div>
                <xsl:apply-templates select="tag/value"/>
                </div>

                <xsl:apply-templates select="annotated_protein/annotation" mode="prsm"/>
                <xsl:apply-templates select="annotated_protein/shift_list" mode="prsm"/>

                <p>
                    <a href="#" onclick="showAllPeaks();">All peaks (<xsl:value-of select="count(ms/peaks/peak)"/>)</a>
                    &#160;
                    <!--a href="#" onclick="showMatchedPeaks();">Matched peaks (<xsl:value-of select="count(ms/peaks/peak[count(matched_ions/matched_ion)>0])"/>)</a-->
                    <a href="#" onclick="showMatchedPeaks();">Matched peaks (<xsl:value-of select="count(ms/peaks/peak[count(matched_ions/matched_ion)>0])"/>)</a>
                    &#160;
                    <a href="#" onclick="showNotMatchedPeaks();">Not matched peaks (<xsl:value-of select="count(ms/peaks/peak[count(matched_ions/matched_ion)=0])"/>)</a>
                </p>
                <div class="outer">
                    <div class="innera">
                        <table border="1" class="sortable">
                            <thead>
                                <xsl:call-template name="peaks-header"/>
                            </thead>
                            <tbody>
                                
                                <xsl:apply-templates select="ms/peaks/peak[count(matched_ions/matched_ion)>0]"/>
                                <xsl:apply-templates select="ms/peaks/peak[count(matched_ions/matched_ion)=0]"/>
                            </tbody>
                        </table>
                    </div>
                </div>

                <xsl:call-template name="navigation"/>
            </body>
        </html>
    </xsl:template>


    <xsl:template name="navigation">
        <p>
             <a href="../proteins.html">All proteins</a> /
             <a href="../proteins/protein{annotated_protein/sequence_id}.html"><xsl:value-of select="annotated_protein/sequence_name"/></a> /
             <a href="../species/species{annotated_protein/species_id}.html">Species # <xsl:value-of select="annotated_protein/species_id"/></a>
        </p>
    </xsl:template>

    <xsl:template match="ms/peaks/peak/matched_ions/matched_ion" mode="peakMatch">
        if (peakId == <xsl:value-of select="../../id"/>)
            return true;
    </xsl:template>

    <xsl:template match="annotated_protein/annotation" mode="prsm">
        <div id="alignment" style="font-family: 'FreeMono', Miltonian, monospace; font-size:16;line-height:2.5;background-color:#FFF">
            <xsl:apply-templates select="character" mode="prsm"/>
        </div>
    </xsl:template>

<xsl:template match="matched_peaks" mode="prsm">
    <xsl:for-each select="matched_peak">
      <xsl:value-of select="peak_id"/><xsl:text>,</xsl:text>
    </xsl:for-each>
</xsl:template>
    <xsl:template match="character" mode="prsm">
        <xsl:if test="type = 'cleavage'">
            <xsl:choose>
                <xsl:when test="position mod 30 = 0 and position = 0">
                    <xsl:if test="position = 0">
                    <br/>
	            <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;&nbsp;&nbsp;]]></xsl:text>
                    <xsl:text>1 </xsl:text>
                    </xsl:if>
                    <!--xsl:if test="position > 0">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;]]></xsl:text>
                    <xsl:value-of select="position"/>
                    <xsl:text disable-output-escaping="yes"><![CDATA[<br/>]]></xsl:text>
                    <br/>
                    <xsl:choose>
                    <xsl:when test="position > 10000">
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 1000">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 100">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 10">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    </xsl:choose>
                    </xsl:if-->
                </xsl:when>
            </xsl:choose>
            <xsl:choose>
                <xsl:when test="cleavage_type = 'species'">
                    <a style="text-decoration:none" href="#">
                        <xsl:attribute name="onclick">
                            showIonPeaks('<xsl:apply-templates select="matched_peaks" mode="prsm"/>')
                        </xsl:attribute>
                        <span style ="color:black;">
                            <xsl:choose>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '0'">
                                    <xsl:choose>
                                      <xsl:when test="cleavage_trunc = ']'">
                                        <span style ="color:red;">
                                        <xsl:text>]</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:when test="cleavage_trunc = '['">
                                        <span style ="color:red;">
                                        <xsl:text>[</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:otherwise>
                                        <xsl:text> </xsl:text>
                                      </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '0'">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23AB;</xsl:text>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '1'">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23A9;</xsl:text>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '1'">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23B1;</xsl:text>
                                </xsl:when>
                            </xsl:choose>
                        </span>
                    </a>
                </xsl:when>
                <xsl:when test="cleavage_type = 'unexpected_shift'">
                    <!--span style ="color:black; background:#F6CECE">
                        <xsl:text> </xsl:text>
                    </span-->
                    <a style="text-decoration:none" href="#">
                         <xsl:attribute name="onclick">
                            showIonPeaks('<xsl:apply-templates select="matched_peaks" mode="prsm"/>')
                        </xsl:attribute>
                        
                            <xsl:choose>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '0'">
                                    <xsl:choose>
                                      <xsl:when test="cleavage_trunc = ']'">
                                        <span style ="color:red;">
                                        <xsl:text>]</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:when test="cleavage_trunc = '['">
                                        <span style ="color:red;">
                                        <xsl:text>[</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:otherwise>
                                       <span style ="color:black; background:#F6CECE">
                                        <xsl:text> </xsl:text>
                                       </span>
                                      </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '0'">
                                  <xsl:if test="shift_no_letter = 0">
                                  <span style ="color:black;">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23AB;</xsl:text>
                                  </span>
                                  </xsl:if>
                                  <xsl:if test="shift_no_letter != 0">
                                  <span style ="color:black;background:#F6CECE">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23AB;</xsl:text>
                                  </span>
                                  </xsl:if>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '1'">
                                  <xsl:if test="shift_no_letter = 0">
                                   <span style ="color:black; ">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23A9;</xsl:text>
                                   </span>
                                  </xsl:if>
                                  <xsl:if test="shift_no_letter != 0">
                                   <span style ="color:black; background:#F6CECE">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23A9;</xsl:text>
                                   </span>
                                  </xsl:if>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '1'">
                                  <xsl:if test="shift_no_letter = 0">
                                   <span style ="color:black;">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23B1;</xsl:text>
                                   </span>
                                  </xsl:if>
                                  <xsl:if test="shift_no_letter != 0">
                                   <span style ="color:black;background:#F6CECE">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23B1;</xsl:text>
                                   </span>
                                  </xsl:if>
                                </xsl:when>
                            </xsl:choose>
                            <xsl:if test="shift_no_letter != 0">
                            <xsl:choose>
                            <xsl:when  test="display_position = '0'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; left:-6pt; top:-24pt; font-size: 8pt; color:red; text-decoration:none;">
                                        <xsl:value-of select="shift_no_letter"/>
                                    </span>
                                </span>
                            </xsl:when>
                            <xsl:when  test="display_position = '1'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; left:-6pt; top:-32pt; font-size: 8pt; color:red; text-decoration:none;">
                                        <xsl:value-of select="shift_no_letter"/>
                                    </span>
                                </span>
                            </xsl:when>
                            </xsl:choose>
                            </xsl:if>
                        
                    </a>
                </xsl:when>
                <xsl:when test="cleavage_type = 'expected_shift'">
                    <!--span style ="color:black;">
                        <xsl:text> </xsl:text>
                    </span-->
                    <a style="text-decoration:none" href="#">
                         <xsl:attribute name="onclick">
                            showIonPeaks('<xsl:apply-templates select="matched_peaks" mode="prsm"/>')
                        </xsl:attribute>
                        
                            <xsl:choose>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '0'">
                                    <xsl:choose>
                                      <xsl:when test="cleavage_trunc = ']'">
                                        <span style ="color:red;">
                                        <xsl:text>]</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:when test="cleavage_trunc = '['">
                                        <span style ="color:red;">
                                        <xsl:text>[</xsl:text>
                                        </span>
                                      </xsl:when>
                                      <xsl:otherwise>
                                       <span style ="color:black;">
                                        <xsl:text> </xsl:text>
                                       </span>
                                      </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '0'">
                                  <span style ="color:black;">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23AB;</xsl:text>
                                  </span>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '0' and exist_c_ion = '1'">
                                   <span style ="color:black; ">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23A9;</xsl:text>
                                   </span>
                                </xsl:when>
                                <xsl:when test="exist_n_ion = '1' and exist_c_ion = '1'">
                                   <span style ="color:black;">
                                    <xsl:text disable-output-escaping="yes">&amp;#x23B1;</xsl:text>
                                   </span>
                                </xsl:when>
                            </xsl:choose>
                        
                    </a>
                </xsl:when>
                <xsl:when test="cleavage_type = 'n_truncation' or cleavage_type = 'c_truncation'">
                        <xsl:text> </xsl:text>
                </xsl:when>

            </xsl:choose>
            <xsl:choose>
                <xsl:when test="position mod 30 = 0 and (exist_n_ion = '1' or exist_c_ion = '1')">
                    <!--xsl:if test="position = 0">
                    <br/>
	            <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;&nbsp;&nbsp;]]></xsl:text>
                    <xsl:text>1 </xsl:text>
                    </xsl:if-->
                    <xsl:if test="position > 0">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;]]></xsl:text>
                    <xsl:value-of select="position"/>
                    <xsl:text disable-output-escaping="yes"><![CDATA[<br/>]]></xsl:text>
                    <br/>
                    <xsl:choose>
                    <xsl:when test="position > 10000">
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 1000">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 100">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    <xsl:when test="position > 10">
                    <xsl:text disable-output-escaping="yes"><![CDATA[&nbsp;&nbsp;&nbsp;]]></xsl:text>
                    <xsl:value-of select="position + 1"/><xsl:text> </xsl:text>
                    </xsl:when>
                    </xsl:choose>
                    </xsl:if>
                </xsl:when>
                <xsl:when test="position > 0 and position mod 10 = 0 and cleavage_type = 'truncation'">
                    <span style ="color:black">
                    <xsl:text disable-output-escaping="yes">&amp;nbsp;&amp;nbsp;</xsl:text>
                    </span>
                </xsl:when>
                <xsl:when test="position > 0 and position mod 10 = 0 and cleavage_type != 'unexpected_shift' and cleavage_type != 'expected_shift'">
                    <span style ="color:black;">
                    <xsl:text disable-output-escaping="yes">&amp;nbsp;&amp;nbsp;</xsl:text>
                    </span>
                </xsl:when>
                <xsl:when test="position > 0 and position mod 10 = 0 and cleavage_type != 'unexpected_shift'">
                    <span style ="color:black;">
                    <xsl:text disable-output-escaping="yes">&amp;nbsp;&amp;nbsp;</xsl:text>
                    </span>
                </xsl:when>
                <xsl:when test="position > 0 and position mod 10 = 0 and cleavage_type != 'expected_shift'">
                    <span style ="color:black; background:#F6CECE">
                    <xsl:text disable-output-escaping="yes">&amp;nbsp;&amp;nbsp;</xsl:text>
                    </span>
                </xsl:when>
            </xsl:choose>
        </xsl:if>
        <xsl:if test="type = 'residue'">
            <xsl:choose>
                <xsl:when test="residue_type = 'n_trunc'">
                    <span style ="color:gray">
                        <xsl:value-of select="acid"/>
                    </span>
                </xsl:when>
                <xsl:when test="residue_type = 'c_trunc'">
                    <span style ="color:gray">
                        <xsl:value-of select="acid"/>
                    </span>
                </xsl:when>
                <xsl:when test="residue_type = 'unexpected_shift'">
                    <xsl:if test="is_modification = '1'">
                        <xsl:choose>
                            <xsl:when  test="display_position = '0'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; top:-24pt; font-size: 8pt; color:red; text-decoration:none;">
                                        <xsl:value-of select="shift"/>
                                    </span>
                                </span>
                            </xsl:when>
                            <xsl:when  test="display_position = '1'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; top:-32pt; font-size: 8pt; color:red; text-decoration:none;">
                                        <xsl:value-of select="shift"/>
                                    </span>
                                </span>
                            </xsl:when>
                        </xsl:choose>
                    </xsl:if>
                    <xsl:choose>
                        <xsl:when  test="is_expected = '0'">
                        <span style ="color:black; background:#F6CECE">
                            <xsl:value-of select="acid"/>
                        </span>
                        </xsl:when>
                        <xsl:when  test="is_expected = '1'">
                        <span style ="color:{shift_style}; background:#F6CECE">
                            <xsl:value-of select="acid"/>
                        </span>
                        </xsl:when>
                    </xsl:choose>
                </xsl:when>
                <xsl:when test="residue_type = 'expected_shift' or is_expected = '1'">
                    <!--xsl:if test="is_modification = '2'">
                        <xsl:choose>
                            <xsl:when  test="display_position = '0'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; top:-32pt; font-size: 8pt; color:blue; text-decoration:none;">
                                        <xsl:value-of select="shift"/>
                                    </span>
                                </span>
                            </xsl:when>
                            <xsl:when  test="display_position = '1'">
                                <span  style="position: relative;">
                                    <span style="position: absolute; top:-48pt; font-size: 8pt; color:blue; text-decoration:none;">
                                        <xsl:value-of select="shift"/>
                                    </span>
                                </span>
                            </xsl:when>
                        </xsl:choose>
                    </xsl:if-->
                    <span style ="color:{shift_style};">
                        <xsl:value-of select="acid"/>
                    </span>
                </xsl:when>
                <xsl:when test="residue_type = 'normal'">
                    <span style ="color:black; ">
                        <xsl:value-of select="acid"/>
                    </span>
                </xsl:when>

            </xsl:choose>
        </xsl:if>

    </xsl:template>

<xsl:template match="shift_list" mode="prsm">
<div>
<br/>
<xsl:text disable-output-escaping="yes">Fixed PTM:&amp;nbsp;</xsl:text>
<xsl:for-each select="shift" >
    <span style ="color:{color};">
       <xsl:value-of select="type"/>
       <xsl:text disable-output-escaping="yes">&amp;nbsp;</xsl:text>
    </span>
</xsl:for-each>
</div>
</xsl:template>


    <xsl:template name="peaks-header">
        <tr>
            <td width="120" class="sortableHeader">Monoisotopic mass</td>
            <td width="100" class="sortableHeader">Monoisotopic m/z</td>
            <td width="80" class="sortableHeader">Intensity</td>
            <td width="70" class="sortableHeader">Charge</td>
            <td width="120" class="sortableHeader">Adjusted mass</td>
            <td width="50" class="sortableHeader">Ion type</td>
            <td width="70" class="sortableHeader">Ion position</td>
            <td width="70" class="sortableHeader">Mass error</td>
            <td width="70" class="sortableHeader">PPM error</td>
        </tr>
    </xsl:template>

    <xsl:template match="peak">
        <tr id="peak{id}">
            <xsl:if test="count(matched_ions/matched_ion) = 0">
                <xsl:attribute name="class">nobreak</xsl:attribute>
            </xsl:if>
            <td align="center" width="120">
		<!--http://xml.apache.org/xalan-c/usagepatterns.html
		This XSLT function includes two or three arguments (the third is optional): number, format pattern, and decimal-format name. Xalan-C++ ignores the format pattern and optional decimal-format name-->
                <!--<xsl:value-of select="format-number(monoisotopic_mass,'0.0000')"/>-->
                <xsl:value-of select="monoisotopic_mass"/>
            </td>
            <td align="center" width="100">
                <!--<xsl:value-of select="format-number(monoisotopic_mz,'0.0000')"/>-->
                <xsl:value-of select="monoisotopic_mz"/>
            </td>
            <td align="center" width="80">
                <!--<xsl:value-of select="format-number(intensity,'0.00')"/>-->
                <xsl:value-of select="intensity"/>
            </td>
            <td align="center" width="70">
                <xsl:value-of select="charge"/>
            </td>
            <td colspan="5" style="border:0">
                <table border="1">
            <xsl:apply-templates select="matched_ions/matched_ion"/>
                </table>
            </td>
        </tr>
    </xsl:template>

    <xsl:template match="matched_ion">
        <tr>
            <td  width="115" align="center">
                <xsl:value-of select="adjusted_mass"/>
            </td>
            <td  width="50" align="center">
                <xsl:value-of select="type"/>
            </td>
            <td  width="70" align="center" sorttable_customkey="{ion_left_position}">
                <!--xsl:value-of select="ion_display_position"/-->
                <xsl:value-of select="ion_display_position"/>
            </td>

            <td  width="70" align="center">
                <!--<xsl:value-of select="format-number(mass_error,'0.0000')"/>-->
                <xsl:value-of select="mass_error"/>
            </td>
            <td  width="69" align="center">
                <!--<xsl:value-of select="format-number(ppm,'0.00')"/>-->
                <xsl:value-of select="ppm"/>
            </td>
        </tr>
    </xsl:template>
</xsl:stylesheet>
