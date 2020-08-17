/**
 * Sets all the fixed ptms to UI under Fixed Ptm column
 */
function setFixedPtmListToUI(){
    let dropDownMenuLink = document.getElementById("dropdownMenuLink");
    
    this.fixedPtmList.forEach(function(fixedPtm){
        let value = fixedPtm.acid+" : "+fixedPtm.mass;
        let option = document.createElement("option");
        option.setAttribute("value",value);
        option.innerHTML = value;
        dropDownMenuLink.appendChild(option);
    })
    $('.addnewrow').click(function(){
        let fixedptm = document.getElementById("dropdownMenuLink").value
        if(fixedptm != "fixed_ptm")
        {
            addNewFixedPtmRow(fixedptm);
        }
    })
}

/**
 * @function addNewFixedPtmRow
 * @description 
 * On Click of '+' symbol under fixed ptms in the HTML, creates new block to add Acid and mass shift
 * @param {String} fixedptm - Contains Acid name and mass shift seperated by ':'
 */
function addNewFixedPtmRow(fixedptm){
    // console.log("fixedptm : ", fixedptm);
    let acid = '' ;
    let mass = '' ;
    if(fixedptm !== "other")
    {
        let splitVal = fixedptm.split(":");
        acid = splitVal[0].trim();
        mass = splitVal[1].trim();
    }
    
    let div = document.getElementById("fixedptmslist");
    let fixedptmsdiv = document.createElement("div");
    fixedptmsdiv.setAttribute("class","fixedptms");
    
    //Creating div with input fixed acid and fixed mass 
    let inputAcid = document.createElement("input");
    inputAcid.setAttribute("type","text");
    inputAcid.setAttribute("class","form-control");
    inputAcid.setAttribute("id","fixedptmacid");
    inputAcid.setAttribute("value",acid);
    
    let span = document.createElement("span");
    span.innerHTML = "&nbsp;:&nbsp;";
    
    let inputMass = document.createElement("input");
    inputMass.setAttribute("type","text");
    inputMass.setAttribute("class","form-control");
    inputMass.setAttribute("id","fixedptmmass");
    inputMass.setAttribute("value",mass);
    
    let span2 = document.createElement("span");
    span2.innerHTML = "&nbsp;";
    
    let addButton = document.createElement("button");
    addButton.setAttribute("type","button");
    addButton.setAttribute("class","form-control btn btn-default btn-sm addnewrow");
    
    let iAddFrame = document.createElement("i");
    iAddFrame.setAttribute("class","fa fa-plus");
    addButton.appendChild(iAddFrame);
    
    let span3 = document.createElement("span");
    span3.innerHTML = "&nbsp;";
    
    let removeButton = document.createElement("button");
    removeButton.setAttribute("type","button");
    removeButton.setAttribute("class","form-control btn btn-default btn-sm removerow");
    
    let iRemoveFrame = document.createElement("i");
    iRemoveFrame.setAttribute("class","fa fa-times");
    removeButton.appendChild(iRemoveFrame);
    
    fixedptmsdiv.appendChild(inputAcid);
    fixedptmsdiv.appendChild(span);
    fixedptmsdiv.appendChild(inputMass);
    fixedptmsdiv.appendChild(span2);
    fixedptmsdiv.appendChild(removeButton);
    div.appendChild(fixedptmsdiv);	
    
    $('.removerow').click(function(){
        let acid = $(this).parent().find("#fixedptmacid").val();
        $(this).parent().remove();
        //temp code
        let errorVal ;
        let errorType = $(".error_dropdown").val();
        if(errorType == "masserror") {
            errorVal = parseFloat($("#errorval").val().trim());
        }
        else {
            errorVal = parseFloat($("#errorval").val().trim());
        }
        // here
        let executionObj = new SeqOfExecution();
        executionObj.sequenceOfExecution(errorType,errorVal,acid);
    })
}

/**
 * Set all the fixed masses on html
 */
function setFixedMasses(fixedPtmList){
    if(fixedPtmList.length !=0)
    {
        let commonfixedPtmList = [{name:"Carbamidomethylation",acid:"C",mass:57.021464},{name:"Carboxymethyl",acid:"C",mass:58.005479}];
        for(let i=0;i<fixedPtmList.length;i++)
        {
            for(let j=0; j<commonfixedPtmList.length;j++)
            {
                if(fixedPtmList[i].name.toUpperCase() ===  commonfixedPtmList[j].name.toUpperCase())
                {
                    let fixedptm = commonfixedPtmList[j].acid + ":" + commonfixedPtmList[j].mass;
                    addNewFixedPtmRow(fixedptm);
                    break;
                }
            }
        }
    } 
}