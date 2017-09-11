//*==============================================================================
//*    Helpware NavScript.js 1.01
//*      Copyright (c) 2008, Robert Chandler, The Helpware Group
//*      http://helpware.net/FAR/
//*      support@helpware.net
//*    Descriptions:
//*      Adds a Open Navigation link at page top if nav is currently not open
//*       or inside a CHM.
//*    Usage: Anyone may use and modify this file. If you modify it then
//*       please change its name and acknowledge my work in your (c) statement.
//*       Email me your changes if you think that others could benefit too.
//*==============================================================================
//*  10-June-2008: 1.00 RWC - Original Version
//*  11-July-2008: 1.01 RWC - Now detects if inside CHM Help file. IsInsideChm()
//*

function GetTest() {
   alert( window.location );
}

function WriteOpenNavLink(navLink) {
  var ss = '<div style="font-family: Verdana; font-size: 80%;color: #333333; margin:0px 0px 0px 0px; padding:0px 0px 0px 0px;">'
          +'<a href="'+navLink+'" style="padding:0px 5px 0px 5px;">&lt; Table Of Contents</a>'
          +'</div>';
  document.write(ss);
}

function IsNavOpen() {
  if ((top.right == null) || (top.right == undefined) || (top.right.location == null) || (top.right.location == undefined) || (typeof(top.right.location.href) != "string") || (top.right.location.href == ""))
    return false;  //no nav found
  else
    return true;  //nav found
}

function IsInsideChm() {   //returns true if current file is inside a CHM Help File
  var ra = /::/;
  return (location.href.search(ra) > 0); //If found then then we are in a CHM
  }

// pass in the directory level. 0 = if this HTML is same level as hh_goto.hh; 1 = of one level down etc
function WriteNavLink(aDirLevel) {
  if ((!IsNavOpen()) && (!IsInsideChm()))
  {
    var prefix = "";
    for (var n=0; n < aDirLevel; n++)
      prefix = prefix + "../";

    //find last back slash in path
    var x = location.href.lastIndexOf("/");       // get last splash of "path/dir/name.htm"
    for (var n=0; n < aDirLevel; n++)
      x = location.href.lastIndexOf("/", x-1);    // get 2nd last slash etc
    var curFileName = location.href.substr(x+1);

    var navLink = prefix + "hh_goto.htm#" + curFileName
    WriteOpenNavLink(navLink);
  }
}



