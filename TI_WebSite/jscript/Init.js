Sys.Application.add_init(AppInit);
Sys.Application.add_load(function(sender, args) { 
           $addHandler(window, 'resize', window_resize); 
       }); 
        
function AppInit(sender) {
    IG_internalAddEvent(window, "resize", window_resize);
    Sys.WebForms.PageRequestManager.getInstance().add_endRequest(End);
}
 
function End(sender, args) {
}