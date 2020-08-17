/**
 * Set Total mass on to the html
 * @param {*} totalMass 
 */
function setTotalSeqMass(totalMass){
    totalMass = totalMass.toFixed(4);
    $("#totalmass").html(totalMass);
}

/**
 * Set Mass difference on to the html
 * @param {Float} precursorMass - Contains Precursor mass
 * @param {Float} proteinMass - Contains calculated protein Mass
 */
function setMassDifference(precursorMass, proteinMass){
    let diff = proteinMass - precursorMass ;
    document.getElementById("massvariation").innerHTML = diff.toFixed(4);
    return (proteinMass - precursorMass);
}