class SvgAAS {
    prsm_data;
    svg_id;

    constructor(prsm_data, svg_id) {
        this.prsm_data = prsm_data;
        this.svg_id = svg_id;
    }

    drawGraph() {
        // Get the parameters to draw the SVG visualization 
        let para = new parameters();

        // Detting prsm data from prsm_data variable. prsm_data is a global variable from the prsm data file
        let prsm = this.prsm_data.prsm ;

        // Draws SVG visualization with acid sequence
        buildSvg(para,prsm,this.svg_id);

        // Color the background of occurence of mass shift from the left position to the right position given in the data
        massShiftBackgroundColor(para,prsm,this.svg_id);

        // Get the amount of skipped acid and write the amount of skipped acid at the start and end of the sequence 
        skippedAcidNotification(para,prsm,this.svg_id) ;

        // If show number attribute is true 
        if(para.show_num)
        {
            // Get the numerical count at the start and end of each row of sequence
            getNumValues(para,prsm,this.svg_id);
        }

        // Determine the start and end position of the sequence
        drawAnnoOfStartEndPosition(para,prsm,this.svg_id) ;

        //	Draw Annotations based prefix and suffix match
        annotations(para,prsm,this.svg_id);

        // Get the position of the fixed ptms and color them to red
        addColorToFixedPtms(prsm,this.svg_id);
    }
}
