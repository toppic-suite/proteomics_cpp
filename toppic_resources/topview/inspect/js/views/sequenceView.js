/**
 * Set Sequence on to html
 */
function setDataToSequence(sequence, unknownMassShiftList){
    let massShiftsObj = new MassShifts();
    let modSequence = massShiftsObj.formSequence(sequence,unknownMassShiftList);
    jqueryElements.sequenceData.val(modSequence);
}