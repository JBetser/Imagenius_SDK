// Fix an issue with FireFox, when postback the modal popup
// flickers along with it. Reseting PopupControl display to none
// solves the problem.
function RemoveMe(control) {
    document.getElementById(control).style.display = "none";
}