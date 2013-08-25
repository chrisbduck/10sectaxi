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
