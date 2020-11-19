const domElements = {
    precursorMass : document.getElementById("precursormass"),
    customControlInput : document.getElementsByClassName("custom-control-input"),
    totalSeqMass : document.getElementById("totalseqmass_h6"),
    massVariation : document.getElementById("massvariation_h6"),
    dropDownMenuLink : document.getElementById("dropdownMenuLink"),
    fixedPtmList : document.getElementById("fixedptmslist"),
    matchedPeaks : document.getElementsByClassName('matched_peak'),
    unmatchedPeaks : document.getElementsByClassName('unmatched_peak'),
    ionTableContainer : document.getElementById("divselectediontablecontainer"),
    massDifference : document.getElementById("massvariation"),
    monoMassTableContainer : document.getElementById("divtableContainer"),
};

const jqueryElements = {
    peakData : $("#peakdata"),
    massData : $("#data"),
    sequenceData : $("#sequencedata"),
    errorDropdown : $('#error_dropdown'),
    errorValue : $("#errorval"),
    errorUnit : $("#errorunit"),
    submit : $("#submit"),
    hideTable : $('#hide_table'),
    monoMassTableContainer : $("#divtableContainer"),
    addFixedPtmRow : $('.addnewrow'),
    fixedPtms : $(".fixedptms"),
    ionTableContainer : $("#divselectediontablecontainer"),
    // matchedFragments : $(".matched_fragments"),
    totalMass : $("#totalmass"),
    monoMassTableBody : $("#tableContainer tbody"),
    monoMassTableColumns : $("#tableContainer thead tr th"),
    // peakRow : $(".peakRows"),
    allPeakCount : $("#all_peak_count"),
    matchedPeakCount : $("#matched_peak_count"),
    unmatchedPeakCount : $("#unmatched_peak_count"),
    peakCount: $("#peakCount"),
}


const COMMON_FIXED_PTM_LIST = [{name:"Carbamidomethylation",acid:"C",mass:57.021464},{name:"Carboxymethyl",acid:"C",mass:58.005479}];
// const commonFixedPtmList = [{name:"Carbamidomethylation",acid:"C",mass:57.021464},{name:"Carboxymethyl",acid:"C",mass:58.005479}];

/**
 * Class contains all the id's and class names used in HTML
 */
class Constants{
    static SEQSVGID = "seqsvg";
    static SEQSVGMODALID = "prsm_popup_svg";
    static SPECTRUMGRAPHID = "spectrum";
    static MONOMASSGRAPHID = "monoMassGraph";
    static PEAKCOUNTID = "peakCount";
    static MASSERROR = "masserror";
    static MASSSHIFT_CLASSID = "massshift_class";
    static TABLECONTAINERID = "tableContainer";
    static FRAGMENTIONTABLECONTAINER = "selectedIonTableContainer";
    static H_FRAGMENTEDTABLE = "h_fragmentedtable";
    static SVGDOWNLOADID = "svg_download";
    static SPECTRUMDOWNLOADID = "spectrum_download";
    static SEQDOWNLOADPNG = "seq_download_png";
    static SEQDOWNLOADSVG = "seq_download_svg";
    static DIVTABLECONTAINER = "divtableContainer";
    static TABLEHEIGHT = "400px";
    static GRAPHDOWNLOAD = "graph_download";
    static GRAPHDOWNLOADPNG = "graph_download_png";
    static GRAPHDOWNLOADSVG = "graph_download_svg";
    static MONOGRAPHDOWNLOAD = "mono_graph_download";
    static MONOGRAPHDOWNLOADPNG = "mono_graph_download_png";
    static MONOGRAPHDOWNLOADSVG = "mono_graph_download_svg";
    static GRAPHTABDIV = "ms2_svg_div";
    static GRAPHTABNAV = "ms2_graph_nav";
}