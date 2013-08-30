//------------------------------------------------------------------------------
// page-fns.js: JavaScript functions to run on page elements.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

// These functions give the app access to page elements

//------------------------------------------------------------------------------

_page_toggleMusic = function()
{
    music_box = document.getElementById("music");
    music_box.checked = !music_box.checked;
}

//------------------------------------------------------------------------------

_page_isMusicEnabled = function()
{
    return document.getElementById("music").checked;
}

//------------------------------------------------------------------------------

_page_toggleDebug = function()
{
    debug_box = document.getElementById("debug");
    debug_box.checked = !debug_box.checked;
    output_box = document.getElementById("output");
    _page_updateDebug();
}

//------------------------------------------------------------------------------

_page_updateDebug = function()
{
    debug_box = document.getElementById("debug");
    output_box = document.getElementById("output");
    
    if (debug_box.checked)
    {
        if (output_box.saved_display !== undefined)
            output_box.style.display = output_box.saved_display;
    }
    else
    {
        output_box.saved_display = output_box.style.display;
        output_box.style.display = 'none';
    }
}

//------------------------------------------------------------------------------

_page_isDebugEnabled = function()
{
    return document.getElementById("debug").checked;
}

//------------------------------------------------------------------------------

_page_onBodyLoad = function()
{
    _page_updateDebug();
}

//------------------------------------------------------------------------------
