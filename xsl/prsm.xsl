<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">
  <xsl:output encoding="UTF-8" method="html" omit-xml-declaration="yes" indent="yes"/>

  <xsl:include href="prsm_anno.xsl"/>

  <xsl:template match="prsm">
    <html>
      <head>
        <title>Protein-Spectrum-Match for Spectrum #<xsl:value-of select="ms/ms_header/id"/> -
          Proteoform #<xsl:value-of select="annotated_protein/proteoform_id"/> 
          of Protein <xsl:value-of select="annotated_protein/sequence_name"/>
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
          function changePosition(){
          var s = document.getElementsByName("0")
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
          width:50em;
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

      <body onload="changePosition()">
        <xsl:call-template name="navigation"/>

        <h3>Protein-Spectrum-Match #<xsl:value-of select="prsm_id"/> for Spectrum #<xsl:value-of select="ms/ms_header/id"/>
        </h3>

        <table cellpadding="3" width="750" class="info">
          <tr>

            <td>PrSM ID: </td>

            <td> <xsl:value-of select="prsm_id"/> </td>

            <td> Scan(s): </td>

            <td> <xsl:value-of select="ms/ms_header/scans"/> </td>

            <td>Precursor charge:</td>

            <td> <xsl:value-of select="ms/ms_header/precursor_charge"/> </td>

          </tr>
          <tr>

            <td> Precursor m/z: </td>

            <td> <xsl:value-of select="ms/ms_header/precursor_mz"/> </td>

            <td>Precursor mass:</td>

            <td> <xsl:value-of select="ms/ms_header/precursor_mass"/> </td>

            <td>Proteoform mass:</td>

            <td> <xsl:value-of select="annotated_protein/proteoform_mass"/> </td>

          </tr>
          <tr>

            <td># matched peaks:</td>

            <td> <xsl:value-of select="matched_peak_number"/> </td>

            <td># matched fragment ions:</td> 
            
            <td> <xsl:value-of select="matched_fragment_number"/> </td>

            <td># unexpected PTMs:</td>

            <td> <xsl:value-of select="annotated_protein/unexpected_change_number"/> </td>

          </tr>
          <tr>
            <td>E-value:</td>

            <td> <xsl:value-of select="e_value"/> </td>

            <td>P-value:</td>

            <td> <xsl:value-of select="p_value"/> </td>

            <td>Spectral FDR:</td>

            <td> <xsl:value-of select="fdr"/> </td>

          </tr>
        </table>
        <br/>

        <xsl:apply-templates select="annotated_protein/annotation" mode="prsm"/> 

        <br/>

        <div>
          <a href="#" onclick="showAllPeaks();">
            <xsl:text>All peaks (</xsl:text>
            <xsl:value-of select="count(ms/peaks/peak)"/>
            <xsl:text>)</xsl:text>
          </a>
          <xsl:text>&#160;</xsl:text>
          <a href="#" onclick="showMatchedPeaks();">
            <xsl:text>Matched peaks (</xsl:text>
            <xsl:value-of select="count(ms/peaks/peak[count(matched_ions/matched_ion)>0])"/>
            <xsl:text>)</xsl:text>
          </a>
          <xsl:text>&#160;</xsl:text>
          <a href="#" onclick="showNotMatchedPeaks();">
            <xsl:text>Not matched peaks (</xsl:text>
            <xsl:value-of select="count(ms/peaks/peak[count(matched_ions/matched_ion)=0])"/>
            <xsl:text>)</xsl:text>
          </a>
        </div>

        <div class="outer">
          <div class="innera">
            <table border="1"  class="sortable">
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

  <xsl:template match="ms/peaks/peak/matched_ions/matched_ion" mode="peakMatch">
    if (peakId == <xsl:value-of select="../../id"/>)
    return true;
  </xsl:template>

  <xsl:template name="navigation">
    <p>
      <a href="../proteins.html">All proteins</a> /
      <a href="../proteins/protein{annotated_protein/sequence_id}.html">
        <xsl:value-of select="annotated_protein/sequence_name"/>
      </a> /
      <a href="../proteoforms/proteoform{annotated_protein/proteoform_id}.html">
        Proteoform #<xsl:value-of select="annotated_protein/proteoform_id"/>
      </a>
    </p>
  </xsl:template>

  <xsl:template match="annotated_protein/annotation" mode="prsm">
    <div id="alignment" style="font-family: 'FreeMono', Miltonian, monospace; font-size:16;line-height:2.5;background-color:#FFF">
      <xsl:variable name="row_residue_num" select="30"/>
      <xsl:variable name="residue_num" select="protein_length"/>
      <xsl:variable name="table_first_row">
        <xsl:variable name="tmp" select="floor(first_residue_position div $row_residue_num)"/>
        <xsl:choose>
          <xsl:when test="$tmp &gt; 0">
            <xsl:value-of select="$tmp - 1"/>
          </xsl:when>
          <xsl:otherwise> 
            <xsl:value-of select="$tmp"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <xsl:variable name="table_total_row_num" select="ceiling($residue_num div $row_residue_num)"/>
      <xsl:variable name="table_last_row">
        <xsl:variable name="tmp" select="floor(last_residue_position div $row_residue_num)"/>
        <xsl:choose>
          <xsl:when test="$table_total_row_num &gt; $tmp + 1">
            <xsl:value-of select="$tmp + 1"/>
          </xsl:when>
          <xsl:otherwise> 
            <xsl:value-of select="$tmp"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <table border="0"  cellspacing="0px" cellpadding="0px">
        <xsl:if test="$table_first_row &gt; 0">
          <tr>
            <td colspan="2"></td>
            <td colspan="62" align="center">
              <xsl:text>... </xsl:text> 
              <xsl:value-of select="$table_first_row * 30"/>
              <xsl:text> amino acid residues are skipped at the N-terminus ...</xsl:text> 
            </td>
            <td colspan="3"></td>
          </tr>
        </xsl:if>
        <xsl:call-template name="add_table_row">
          <xsl:with-param name="i" select="$table_first_row"/>
          <xsl:with-param name="table_last_row" select="$table_last_row"/>
        </xsl:call-template>

        <xsl:if test="$table_last_row * 30 + 30 &lt; $residue_num">
          <tr> <td>&#160; </td></tr>
          <tr>
            <td colspan="2"></td>
            <td colspan="62" align="center">
              <xsl:text>... </xsl:text> 
              <xsl:value-of select="$residue_num - $table_last_row * 30 - 30"/>
              <xsl:text> amino acid residues are skipped at the C-terminus ...</xsl:text> 
            </td>
            <td colspan="3"></td>
          </tr>
        </xsl:if>
      </table>         
    </div>

    <div>
      <xsl:variable name="fix_ptm_num" select="count(expected_change[change_type=1])"/>
      <xsl:if test="$fix_ptm_num &gt; 0">
        <br/>
        <xsl:text>Fixed PTMs: </xsl:text> 
        <xsl:apply-templates select="expected_change[change_type=1]"/>
        <br/>
      </xsl:if>
      <xsl:variable name="variable_ptm_num" select="count(expected_change[change_type=2 or change_type = 3])"/>
      <xsl:if test="$variable_ptm_num &gt; 0">
        <br/>
        <xsl:text>Variable PTMs: </xsl:text> 
        <xsl:apply-templates select="expected_change[change_type=2 or change_type =3]"/>
        <br/>
      </xsl:if>
    </div>
  </xsl:template>

  <xsl:template match="cleavage">
    <xsl:text disable-output-escaping="yes"><![CDATA[<span style="]]></xsl:text>
      <xsl:if test="cleavage_type = 'seq_start'">
        <xsl:text>font-weight:bold;color:red;</xsl:text>
      </xsl:if>
      <xsl:if test="cleavage_type = 'seq_end'">
        <xsl:text>font-weight:bold;color:red;</xsl:text>
      </xsl:if>

      <xsl:if test="is_unexpected_change = '1'">
        <xsl:if test="unexpected_change_color = 0">
          <xsl:text>background:#DFFFFF;</xsl:text>
        </xsl:if>
        <xsl:if test="unexpected_change_color = 1">
          <xsl:text>background:#CECEF6;</xsl:text>
        </xsl:if>
      </xsl:if>
      <xsl:text disable-output-escaping="yes"><![CDATA[">]]></xsl:text>

      <xsl:choose>
        <xsl:when test="exist_n_ion = '0' and exist_c_ion = '0'">
          <xsl:choose>
            <xsl:when test="cleavage_type = 'seq_start'">
              <xsl:text>]</xsl:text>
            </xsl:when>
            <xsl:when test="cleavage_type = 'seq_end'">
              <xsl:text>[</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>&#160;</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <a style="text-decoration:none" href="#">
            <xsl:attribute name="title"> 
              <xsl:apply-templates select="matched_peaks" mode="title"/>
            </xsl:attribute>
            <xsl:attribute name="onclick"> 
              <xsl:text>showIonPeaks('</xsl:text>
              <xsl:apply-templates select="matched_peaks" mode="prsm"/>
              <xsl:text>')</xsl:text>
            </xsl:attribute>
            <xsl:choose>
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
          </a>
        </xsl:otherwise>
      </xsl:choose>

      <xsl:text disable-output-escaping="yes"><![CDATA[</span>]]></xsl:text>
  </xsl:template>

  <xsl:template name="peaks-header">
    <tr>
      <td width="100" class="sortableHeader">Monoisotopic mass</td>
      <td width="100" class="sortableHeader">Monoisotopic m/z</td>
      <td width="80" class="sortableHeader">Intensity</td>
      <td width="70" class="sortableHeader">Charge</td>
      <td width="103" class="sortableHeader">Theoretical mass</td>
      <td width="50" class="sortableHeader">Ion</td>
      <td width="70" class="sortableHeader">position</td>
      <td width="70" class="sortableHeader">Mass error</td>
      <td width="73" class="sortableHeader">PPM error</td>
    </tr>
  </xsl:template>

  <xsl:template match="peak">
    <tr id="peak{id}">
      <xsl:if test="count(matched_ions/matched_ion) = 0">
        <xsl:attribute name="class">nobreak</xsl:attribute>
      </xsl:if>
      <td align="center" width="100">
        <xsl:value-of select="monoisotopic_mass"/>
      </td>
      <td align="center" width="100">
        <xsl:value-of select="monoisotopic_mz"/>
      </td>
      <td align="center" width="80">
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
      <td  width="95" align="center">
        <xsl:value-of select="theoretical_mass"/>
      </td>
      <td  width="50" align="center" sorttable_customkey="{ion_sort_name}">
        <xsl:value-of select="ion_type"/>
        <xsl:value-of select="ion_display_position"/>
      </td>
      <td  width="70" align="center">
        <xsl:value-of select="ion_left_position"/>
      </td>

      <td  width="70" align="center">
        <xsl:value-of select="mass_error"/>
      </td>
      <td  width="70" align="center">
        <xsl:value-of select="ppm"/>
      </td>
    </tr>
  </xsl:template>

</xsl:stylesheet>

