// Copyright В© 2009-2016, Rob Chandler.
// Please don't use this file without purchasing FAR. http://helpware.net/FAR/
// This effectively licenses you to use the code. The original TOC code (before modification) was 
// first written by Jean-Claude Manoli (see original copyright below).
// Changes
// RWC: 2005-04-01 - Fix image pre-load section. Last line had typo causing some images not to load
// RWC: 2005-05-21 - Some work fixing TOC Sync
// RWC: 2008-01-30 - Change resizeTree() to be compatible with non-MS browsers
// RWC: 2009-06-10 - All files now saved in UTF-8 file format.
// RWC: 2009-09-26 - Allow Opera browser to scroll the tree when syncing TOC.
// RWC: 2011-09-10 - Fix Sync for \\server\ UNC paths.
// RWC: 2011-09-11 - Fix Sync for CJK paths.
// RWC: 2012-09-04 - Added selectNext(fwd) & findLinkNode()
// RWC: 2013-11-01 - hh_start.hh now passes a parameter to hh_toc.htm using a #bookmark
// RWC: 2015-12-02 - HTML5 requires "px" when setting width and height
// RWC: 2016-01-12 - Added TOC filtering


/* Original Copyright В© 2002 Jean-Claude Manoli [jc@manoli.net]
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author(s) be held liable for any damages arising from
 * the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software
 *      in a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 * 
 *   2. Altered source versions must be plainly marked as such, and must not
 *      be misrepresented as being the original software.
 * 
 *   3. This notice may not be removed or altered from any source distribution.
 */ 



var treeSelected = null; //last treeNode clicked

//pre-load tree nodes images
var imgPlus = new Image();
imgPlus.src="treenodeplus.gif";
var imgMinus = new Image();
imgMinus.src="treenodeminus.gif";
var imgDot = new Image();
imgDot.src="treenodedot.gif";    //rwc - fixed. Was... imgPlus.src="treenodedot.gif";


function findNode(el)
{
// Takes element and determines if it is a treeNode.
// If not, seeks a treeNode in its parents.
	while (el != null)
	{
		if (el.className == "treeNode")
		{
			break;
		}
		else
		{
			el = el.parentNode;
		}
	}
	return el;
}


function clickAnchor(el)
{
// handles click on a TOC link
//
	expandNode(el.parentNode);
	selectNode(el.parentNode);
	el.blur();
}

function findLinkNode(node)
{
	if (node === null || node === undefined)
		node = treeSelected;
	node = findNode(node);
	if (node == null)  
		return null;
	var anchors = node.getElementsByTagName('A');
	if (anchors.length > 0)
		return anchors[0];
	return null;
}


function selectNext(fwd)
{
// Sync forward or back from current selected. Return href of newly selected node.
//
	var el;
	var aref = "";
    var node = document.getElementById('treeRoot');
	var anchors = node.getElementsByTagName('A');

	//nothing selected? - Select the first node
	if (treeSelected == null)  
	{
		if (anchors.length > 0 && anchors[0] != null && anchors[0] != undefined)
		{
			el = anchors[0];
			selectAndShowNode(el);
			aref = el.getAttribute('href');
		}
	}
	else //select the next node
	{
		for(var i = 0; i < anchors.length; i++)
		{
			el = anchors[i];
			if (findNode(el) == treeSelected)  // find the current selected node & walk fwd or back
			{
				if (fwd) el = anchors[i+1];
				else     el = anchors[i-1];
				if (el != null && el != undefined)
				{
					selectAndShowNode(el);	
					aref = el.getAttribute('href');
				}
				break;		
			}
		}
	}
	return aref;
}


function selectNode(el)
{
// Un-selects currently selected node, if any, and selects the specified node
//
	if (treeSelected != null)
	{
		setSubNodeClass(treeSelected, 'A', 'treeUnselected');
	}
	setSubNodeClass(el, 'A', 'treeSelected');
	treeSelected = el;
}


function setSubNodeClass(el, nodeName, className)
{
// Sets the specified class name on el's first child that is a nodeName element
//
	var child;
	for (var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.nodeName == nodeName)
		{
			child.className = className;
			break;
		}
	}
}


function expandCollapse(el)
{
//	If source treeNode has child nodes, expand or collapse view of treeNode
//
	if (el == null)
		return;	//Do nothing if it isn't a treeNode
		
	var child;
	var imgEl;
	for(var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.src)
		{
			imgEl = child;
		}
		else if (child.className == "treeSubnodesHidden")
		{
			child.className = "treeSubnodes";
			imgEl.src = "treenodeminus.gif";
			break;
		}
		else if (child.className == "treeSubnodes")
		{
			child.className = "treeSubnodesHidden";
			imgEl.src = "treenodeplus.gif";
			break;
		}
	}
}


function expandNode(el)
{
//	If source treeNode has child nodes, expand it
//
	var child;
	var imgEl;
	for(var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.src)
		{
			imgEl = child;
		}
		if (child.className == "treeSubnodesHidden")
		{
			child.className = "treeSubnodes";
			imgEl.src = "treenodeminus.gif";
			break;
		}
	}
}

function GetUnixPath(url)
{
	var path = url.replace(/\\/g, '/');         // DOS to Unix slash
	path = path.replace(/\/\/\//, "//");        // Force 2 slashes xxx://xxx
	path = path.replace(/\/\/\//, "//");
	path = path.replace(/\/\/\//, "//");
	path = path.replace(/\/\/\//, "//");
	return path;
}


function GetbaseURL() {
    var url = location.href; 
    var i = url.indexOf("#"); 
    if (i > 0)
        url = url.substr(0, i);

    //RWC 2005-05-21 - This is the real URL base of the TOC
    var gbase = GetUnixPath(url);
    gbase = decodeURI(gbase);
    gbase = gbase.substr(0, gbase.lastIndexOf('/') + 1);   //trim off file name. Leave trailing /
    return gbase;
}

var _BaseUrl = GetbaseURL();


function syncTree(href)
{
    // Selects and scrolls into view the node that references the specified URL
    //
	var loc = new String();
	loc = GetUnixPath(href);
	loc = encodeURI(loc); //encode as valid URI
    //RWC 2005-05-21 - properly Scrub URL of encoding
    loc = decodeURI(loc);  //Converts %2520 -> %20  (under FireFox)
    loc = decodeURI(loc);  //Converts %20 = ' '

	var tocEl = findHref(document.getElementById('treeRoot'), loc, _BaseUrl);
	if (tocEl != null)
	{
		selectAndShowNode(tocEl);
	}
}

function findHref(node, href, base)
{
// find the <a> element with the specified href value
//
    //RWC 24/3/2006: Consider any bookmark on the URL to test
    var href_BaseURL = '';
    var iBookmark = href.indexOf('#');
    if (iBookmark > 0)
        href_BaseURL = href.substr(0, iBookmark);


	var el;
	var anchors = node.getElementsByTagName('A');
	for (var i = 0; i < anchors.length; i++)
	{
		el = anchors[i];
		var aref = new String();
		aref = el.getAttribute('href');
		
		if ((aref.substring(0, 7) != 'http://') 
		&& (aref.substring(0, 8) != 'https://')
		&& (aref.substring(0, 7) != 'file://'))
		{
			aref = base + aref;
		}

		aref = GetUnixPath(decodeURI(aref));
        //RWC: If href has #bookmark and aref does not then compare without bookmarks
        if ((href_BaseURL.length > 0) && (aref.indexOf('#') < 0))
            if (aref == href_BaseURL)
                return el;

		if (aref == href)
		{
			return el;
		}
	}
	return null;
}


function selectAndShowNode(node)
{
// Selects and scrolls into view the specified node
//
	var el = findNode(node);
	if (el != null) 
	{
		selectNode(el);
		do 
		{
			expandNode(el);
			el = findNode(el.parentNode);
		} while ((el != null))  
		
		//vertical scroll element into view
		var windowTop;
		var windowBottom;
		var treeDiv = document.getElementById('tree');
		
		var ua = window.navigator.userAgent.toLowerCase();
		if ((i = ua.indexOf('msie')) != -1)
		{
			windowTop = node.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else if (ua.indexOf('gecko') != -1)
		{
			windowTop = node.offsetTop - treeDiv.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else if (ua.indexOf('opera') != -1)
		{
			windowTop = node.offsetTop - treeDiv.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else 
		{
			return;
		}
		
		if (windowTop < 0)
		{
			treeDiv.scrollTop += windowTop - 18;
			return;
		}
		if (windowBottom < 0)
		{
			treeDiv.scrollTop -= windowBottom - 18;
			return;
		}
	}
}

function resizeTree() 
{
	var treeDiv = document.getElementById("tree");
	var DivFooter = document.getElementById("DivFooter");
  var xTop = treeDiv.offsetTop;
	if (DivFooter)
	    xTop = xTop + DivFooter.offsetHeight;
	treeDiv.style.width = GetDocWidth() + "px";
	var HH = GetDocHeight(); 
	if (HH - xTop > 0)
	    treeDiv.style.height = (HH - xTop) + "px";
}

// old original func
//function resizeTree()
//{
//	var treeDiv = document.getElementById('tree');
//	//treeDiv.setAttribute('style', 'width: ' + document.body.offsetWidth + 'px; height: ' + (document.body.offsetHeight - 27) + 'px;');
//	treeDiv.style.width = document.documentElement.offsetWidth;
//	treeDiv.style.height = document.documentElement.offsetHeight - 27;
//}

//---- New for FAR 5.7 ----
// _toc hierarchical object represents the Table of Contents

var _toc = [];
var _toc_flat = [];

function init_tocNodeObj(tocObj, treeNodeDiv, level, parentObj) {
    tocObj.treeNode = treeNodeDiv;  // DIV treeNode
    tocObj.xParent = parentObj;     // DIV parent - root items = null
    tocObj.xDiv = null;             // DIV treeSubnodes (expando div)
    tocObj.xImg = null;             // IMG (node image)
    tocObj.xCap = null;             // <a> or <span> node with text
    tocObj.xLink = null;            // <a> href="link" - relative link no bookmark
    tocObj.xLevel = level;          // TOC level 1..n
    tocObj.caption = "";            // text of <a> or <span> node 
    tocObj.child = [];              // child nodes
    tocObj.match = false;           // text match
    tocObj.matchUp = false;         // text match found in children
    tocObj.srMatch = false;         // match via search results
    
    _toc_flat[_toc_flat.length] = tocObj; //add to flat list
}

function fill_tocNodeObj(el) {
    var tNode = el.treeNode;
    for (var i = 0; i < tNode.childNodes.length; i++) { 
        var cnode = tNode.childNodes[i];
        
        if (cnode.nodeName == "DIV") {  //expando node
            el.xDiv = cnode;
            
            // Add new child nodes
            if (el.xDiv !== null && el.xDiv !== undefined && el.xDiv.childNodes !== null && el.xDiv.childNodes !== undefined) {  
                var cNodes = el.xDiv.childNodes;
                for (var n = 0; n < cNodes.length; n++) { 
                    if (cNodes[n].nodeName == "DIV") {
                        var cl = el.child.length;
                        el.child[cl] = new Object();
                        init_tocNodeObj(el.child[cl], el.xDiv.childNodes[n], el.xLevel+1, el);
                    }
                }
            }
        }
        else if (cnode.nodeName == "IMG")  //expando image
            el.xImg = cnode;
        else if (cnode.nodeName == "SPAN") { // text field
            el.xCap = cnode;
            if (el.xCap)
               el.caption = decodeHTMLEntities(el.xCap.innerHTML); 
        }
        else if (cnode.nodeName == "A") { // text field & Link
            el.xCap = cnode;
            if (el.xCap) {
                el.caption = decodeHTMLEntities(el.xCap.innerHTML); 
                if (el.xCap.href)
                    el.xLink = MakeRelPath_NoBM(el.xCap.href);
            }
        }
    }
}

function decodeHTMLEntities(text) {
    var entities = [['apos', '\''],['amp', '&'],['lt', '<'],['gt', '>']];
    for (var i = 0, max = entities.length; i < max; ++i) 
        text = text.replace(new RegExp('&'+entities[i][0]+';', 'g'), entities[i][1]);
    return text;
}

function MakeRelPath_NoBM(url) {
    url = GetUnixPath(decodeURI(url));
    var i = url.indexOf("#"); 
    if (i > 0)
        url = url.substr(0, i);  //strip bookmark off
    
    //starts with window base path? Remove the base
    if (url.indexOf(_BaseUrl) == 0)  
        url = url.substr(_BaseUrl.length);
    //alert(url +"\n" + _BaseUrl); //aaaaaaaa
    return url;
}

function build_toc() {
    var treeRoot = document.getElementById('treeRoot');
        
    for (var i = 0; i < treeRoot.childNodes.length; i++) {
        if (treeRoot.childNodes[i].nodeName == "DIV") {
            var idx = _toc.length;
            _toc[idx] = new Object(); //add new array item
            init_tocNodeObj(_toc[idx], treeRoot.childNodes[i], 1, null);
        }
    }
    
    var i = 0;
    while (i < _toc_flat.length) {  
        fill_tocNodeObj(_toc_flat[i]);
        i++;
    }
}

function debugOut_toc() {
    var s = "";
    for (var i = 0; i < _toc_flat.length; i++) {
            s = s + _toc_flat[i].xLevel + ">" + _toc_flat[i].treeNode.nodeName + ", " + _toc_flat[i].treeNode.className + ", caption="+_toc_flat[i].caption + ", childnodes=" + _toc_flat[i].child.length + "\n";
    }
    alert(s);
}

function flattoc_Expand(i) {  //pass toc flat index
	if (_toc_flat[i].xDiv !== null && _toc_flat[i].xDiv.className == "treeSubnodesHidden") {
		_toc_flat[i].xDiv.className = "treeSubnodes";
        _toc_flat[i].xImg.src = "treenodeminus.gif";
	}
}
function flattoc_Collapse(i) {  //pass toc flat index
	if (_toc_flat[i].xDiv !== null && _toc_flat[i].xDiv.className == "treeSubnodes") {
        _toc_flat[i].xDiv.className = "treeSubnodesHidden";
		_toc_flat[i].xImg.src = "treenodeplus.gif";
	}
}

//<span class='filterHighlight'>test</span>
//<span class='searchHighlight'>test</span>
var markTagStart_filter="<span class='filterHighlight'>";  //strings must must be same length
var markTagStart_search="<span class='searchHighlight'>";
var markTagEnd="</span>";

function markNode(i, mark, searchColor) {
    var idx = _toc_flat[i].xCap.innerHTML.indexOf('span');
    if (mark && idx == -1) {
        if (searchColor)
            _toc_flat[i].xCap.innerHTML = markTagStart_search + _toc_flat[i].xCap.innerHTML + markTagEnd;
        else
            _toc_flat[i].xCap.innerHTML = markTagStart_filter + _toc_flat[i].xCap.innerHTML + markTagEnd;
    }
    else if (!mark && idx >= 0) {
        var s = _toc_flat[i].xCap.innerHTML;
        _toc_flat[i].xCap.innerHTML = s.substring(markTagStart_filter.length, s.length-markTagEnd.length);
    }
}


function srFilesMatch(xLink, srFiles) {
    for (var i = 0; i < srFiles.length; i++)
    {
        //alert(i+"> " + srFiles[i] + "\n xLink=" + xLink);
        if (xLink && xLink == srFiles[i])   //match?
            return true;
    }
    return false;
}

function treeFilter(filter, sr) {  // if search results used then 'sr' contains a list of SearchResults

    if (_toc_flat.length == 0)
      build_toc();
    
    //reset colors
    for (var i = 0; i < _toc_flat.length; i++)
    {
        markNode(i, false, false);
        _toc_flat[i].match = false;
        _toc_flat[i].matchUp = false;
        _toc_flat[i].srMatch = false;
    }
    
    //somthing to filter by
    var sFilter = filter.trim().toLowerCase();
    if (sFilter == 0 || _toc_flat.length == 0)
        return;

    //Chars that break words in an entry field   //RWC002 - Space, Ideographic Space
    var breakChars = new RegExp( "[\u0020\u3000]{1}", "gi" );    

    //Split into words
    var words = sFilter.split(breakChars);
    var c = 0;
    for (var i = 0; i < words.length; i++)
        if (words[i] != "") { words[c] = words[i].toLowerCase(); c++; }
    words.length = c;
    
    
    // Using Search Results 'sr' to filter? Get list of sr file names
    var srFiles = [];
    var srMode = (sr && sr.length > 0 && SearchFiles !== "undefined");
    if (srMode) {  
        for (var i = 0; i < sr.length; i++) {
            var idx = sr[i];
            srFiles[i] = SearchFiles[idx];
        }
    }

    // find matches
    for (var i = 0; i < _toc_flat.length; i++) {
        _toc_flat[i].match = false;
        _toc_flat[i].matchUp = false;
        _toc_flat[i].srMatch = false;
        if (srMode) {
            _toc_flat[i].srMatch = srFilesMatch(_toc_flat[i].xLink, srFiles); 
            _toc_flat[i].match = _toc_flat[i].srMatch; 
        }
        if (!_toc_flat[i].match) // then check for simple filter match
            _toc_flat[i].match = (_toc_flat[i].caption.toLowerCase().indexOf(sFilter) >= 0);
        // search for individual words
        if (!_toc_flat[i].match && words.length > 1) { 
            var andMatch = false;
            for (var k = 0; k < words.length; k++) {
                andMatch = (_toc_flat[i].caption.toLowerCase().indexOf(words[k]) >= 0);
                if (!andMatch)
                    break;
            }
            _toc_flat[i].match = andMatch;
        }
    }
    
    //set matchUp - working backwards
    for (var i = _toc_flat.length-1; i >= 0; i--) {
        if ((_toc_flat[i].match || _toc_flat[i].matchUp) && _toc_flat[i].xParent !== null)
            _toc_flat[i].xParent.matchUp = true;
    }
    
    // Expand and highlight matches
    for (var i = 0; i < _toc_flat.length; i++) {
        if (_toc_flat[i].matchUp && _toc_flat[i].child.length !== 0)
            flattoc_Expand(i);
        else 
            flattoc_Collapse(i); 
        if (_toc_flat[i].match) {
            markNode(i, true, _toc_flat[i].srMatch);
        }
    }
    
}

