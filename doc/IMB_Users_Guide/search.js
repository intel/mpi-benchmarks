//*==============================================================================
//*    ** DO NOT REMOVE OR MODIFY THIS COPYRIGHT MESSAGE **
//*
//*    Helpware Search 1.2
//*    Copyright (c) 2004-2016 The Helpware Group
//*    http://helpware.net/FAR/
//*    Requires a Modern Browser that supports JavaScript such as Firefox/IE4/GoogleChrome/Opera/Safari/Netscape
//*    WARNING: You must purchase a copy of FAR HTML v5 or greater to use this file.
//*
//*==============================================================================
//*  31-May-2005: RWC - Fixed Offby one error in highlighting. First word would not highlight. 
//*  10-June-2009: RWC - All files now saved in UTF-8 file format. Search is now Unicode based. Previously ANSI based.
//*     - Added reBreakChars to allow multiple text break chars. Was just space char (0x20).
//*  12-Sept-2011: RWC - Search highlighting now works for all browsers (previously just Internet Explorer).
//*  04-Sept-2012: RWC - Select first item when result list loads.
//*  01-Nov-2013: RWC - Added Try-Catch blocks since exceptions happen if browser security block inter-frame messages.
//*  01-Jan-2016: RWC - Title matches added to top of file. TOC Search can do Title Search Only
//*

//var SearchFiles = ["index.htm","Search_OzNet.html"...
//var SearchTitles =["Molecular products","OzNet Web"...
//var SearchIndexes = [["aa1",[128,129,256,257,323]]...     SearchIndexes[n][0] = "word"  SearchIndexes[n][1] = array of file/title indexes
// ...,["WATER;",[355,361]],["WATER-CIRCULATOR",[383]],...


//Options
var PARAM_PartialMatchOK = true;
var PARAM_TargetWindow = 'content';

//Globals - SearchResults is an array of Page Indexes
var SearchResults = [];
var SearchTitles_UpperCase = [];
var gFindList = [];
var gFirstFindCall = true;

//Chars that break words in an entry field   //RWC002 - Space, Ideographic Space
var reBreakChars = new RegExp( "[\u0020\u3000]{1}", "gi" );    
var cBreakChars = "\u0020\u3000".split(''); //array of 2 chars

//------------------------------------------------------------------------------
// Get Operator Type
//   text should be Uppercase. Return 0 = normal search text
//------------------------------------------------------------------------------
var OPT_AND = 1;
var OPT_OR  = 2;
var OPT_NOT = 3;
function xxGetOpType(text) {
  if ((text=="NOT")||(text=="!")) return OPT_NOT;
  else if ((text=="AND")||(text== "&")||(text== "+")) return OPT_AND;
  else if ((text=="OR")||(text== "|")) return OPT_OR;
  else return(0);
  }


//----------------------------------------------------------------------------
// ProcessSearchTerms()
//----------------------------------------------------------------------------
// Params
//   ss -- string of terms to parse and find
//   DefaultOp - Search Operator to default to for each list term (OPT_OR, OPT_AND, OPT_NOT)
// Desc
//   Parse ss string --> String list. Default Return.
//   Items forced Uppercase (since Database and all calls use uppercase for speed)
//   User can insert override 'AND', 'OR', 'NOT' into the list of terms to
//   alter how the next item is searched. After that we go back to Defaultop.
// Optimization
//   Pass in SearchIndexes + SearchResults arrays (by ref) so no global lookups - speed up loops
//----------------------------------------------------------------------------
function ProcessSearchTerms(ss, DefaultOp) {
  if (ss == null || ss.trim() == "")    
      return;

  //----------------------------------------
  // Remove empty list entried due to multiple spaces passed to split()
  // Force all items to Uppercase
  //----------------------------------------
  var items = ss.split(reBreakChars);  //break into search items
  var c = 0;
  for (var i = 0; i < items.length; i++)
    if (items[i] != "") { items[c] = items[i].toUpperCase(); c++; }
  items.length = c;

  var CheckOp = true;
  var otype = DefaultOp;
  for (var i = 0; i < items.length; i++) {

    //----------------------------------------
    // Check for Override Operators.
    // Don't allow Op override if working with NOT terms
    //----------------------------------------
    if ((CheckOp) && (DefaultOp != OPT_NOT)) {
      otype = xxGetOpType(items[i]);
      CheckOp = (otype == 0);
      if (CheckOp) otype = DefaultOp;
      else continue;
    }
    CheckOp = true;

    //----------------------------------------
    // Find Text results ==> SearchResults
    //----------------------------------------
    if (otype==OPT_OR)  FindText_OR(items[i], SearchIndexes, SearchResults);
    if (otype==OPT_AND) FindText_AND(items[i], SearchIndexes, SearchResults);
    if (otype==OPT_NOT) FindText_DEL(items[i], SearchIndexes, SearchResults);

    //build list of find words
    if (DefaultOp!=OPT_NOT)
      gFindList[gFindList.length] = items[i];

    //Clear global flag
    gFirstFindCall = false;
  }
}


//----------------------------------------------------------------------------

function SearchResults_IndexOf(value) {
    var len = SearchResults.length;
    for (var i = 0; i < len; i++)
        if (SearchResults[i] == value) //match
            return i;
    return -1;
}

function SearchResults_AddUnique(value) {
    if (SearchResults_IndexOf(value) < 0)  // not found in list
        SearchResults[SearchResults.length] = value;   //add it 
}

function SearchResults_Remove(value) {
    var x = SearchResults_IndexOf(value);
    if (x < 0) return; //not found
        
    var len = SearchResults.length;
    for (var i = x; i < len-1; i++)
        SearchResults[i] = SearchResults[i+1];
    SearchResults.length = len - 1;
}

function FindText_Partial(ss, find, partalMatch) {
    var iRet = ss.indexOf(find);
    if (partalMatch || iRet < 0 || ss.length == find.length)  // partial, no match, or full match
        return iRet;

    //check for Full Match
    var c1 = ss.charAt(iRet - 1); //char before match -- '' if before string
    var c2 = ss.charAt(iRet+find.length); //char after match -- '' if off end of string
    if ((c1 == cBreakChars[0] || c1 == cBreakChars[1] || c1 == '')
     && (c2 == cBreakChars[0] || c2 == cBreakChars[1] || c2 == ''))
        return iRet;
    return -1; // not a full match
}

//----------------------------------------------------------------------------
// ProcessTitleSearch()
//   ss -- search text string.
// Desc
//   This one uses the who string to find a title match (ranking hits) 
//   and does parses into terms to search the title again if no match.
//----------------------------------------------------------------------------
function ProcessTitleSearch(ss, DefaultOp) {
  if (ss == null || ss.trim() == "")    
      return;

  //----------------------------------------
  // Get params & remove empty list entried due to multiple spaces passed to split()
  // Force all items to Uppercase
  //----------------------------------------
  var items = ss.split(reBreakChars); //break into search items
  var c = 0;
  for (var i = 0; i < items.length; i++)
    if (items[i] != "") { items[c] = items[i].toUpperCase(); c++; }
  items.length = c;
    
  ss = ss.toUpperCase(); 
    
  //----------------------------------------
  // UpperCase Titles
  //----------------------------------------
  if (SearchTitles_UpperCase.length !== SearchTitles.length) { //do once only
    SearchTitles_UpperCase.length = SearchTitles.length;
    for (var i = 0; i < SearchTitles.length; i++)
        SearchTitles_UpperCase[i] = SearchTitles[i].toUpperCase();
  }
    
  //----------------------------------------
  // Any Title match at start of string
  //----------------------------------------
  for (var i = 0; i < SearchTitles_UpperCase.length; i++)
      if (FindText_Partial(SearchTitles_UpperCase[i], ss, PARAM_PartialMatchOK) == 0)
          SearchResults_AddUnique(i);

  //----------------------------------------
  // Any Title match after start of string?
  //----------------------------------------
  for (var i = 0; i < SearchTitles_UpperCase.length; i++)
      if (FindText_Partial(SearchTitles_UpperCase[i], ss, PARAM_PartialMatchOK) > 0)
          SearchResults_AddUnique(i);
    
  //----------------------------------------
  // Find ss items in titles
  //----------------------------------------

  for (var i = 0; i < SearchTitles_UpperCase.length; i++) 
  {
      var CheckOp = true;
      var otype = DefaultOp;
      var flag = 0;
      
      for (var j = 0; j < items.length; j++) 
      {
            //----------------------------------------
            // Check for Override Operators.
            // Don't allow Op override if working with NOT terms
            //----------------------------------------
            if ((CheckOp) && (DefaultOp != OPT_NOT)) {
              otype = xxGetOpType(items[j]);
              CheckOp = (otype == 0);
              if (CheckOp) otype = DefaultOp;
              else continue;
            }
            CheckOp = true;
          
            //----------------------------------------
            // Text match
            //----------------------------------------
            if (j == 0) // first match
            {
                if (FindText_Partial(SearchTitles_UpperCase[i], items[j], PARAM_PartialMatchOK) >= 0)
                    flag = 1;
                //alert("1>" + SearchTitles_UpperCase[i] +", " + items[j] + ", flag="+flag);
            }
            else
            {
                if (otype==OPT_OR) { 
                    //Any match? - add to list
                    if (flag == 0 && FindText_Partial(SearchTitles_UpperCase[i], items[j], PARAM_PartialMatchOK) >= 0) {
                        flag = 1;
                        //alert("2>" + SearchTitles_UpperCase[i] +", " + items[j] + ", flag=" + flag);
                    }
                }
                if (otype==OPT_AND) { 
                    //no match? - so remove from list 
                    if (flag == 1 && FindText_Partial(SearchTitles_UpperCase[i], items[j], PARAM_PartialMatchOK) < 0) {
                        flag = 0;
                        //alert("3>" + SearchTitles_UpperCase[i] +", " + items[j] + ", flag="+flag);
                    }
                }
                if (otype==OPT_NOT) {
                    //Any match - remove from list
                    if (FindText_Partial(SearchTitles_UpperCase[i], items[j], PARAM_PartialMatchOK) >= 0)
                        flag = -1;
                }
            }
      }
      if (flag == 1)
        SearchResults_AddUnique(i);
      else if (flag == -1)
        SearchResults_Remove(i);
  }
//alert("final titles = " + SearchResults.length);    
    
    
}


//------------------------------------------------------------------------------
// s1 - Any words (OR) -->> one or more words present in a document is a result. Ie. Get the OR of all word search results.
// s2 - All words (AND) -->> all words must be present in each result document. Ie. Get the AND of all word search results.
// s3 - Not these words (NOT) -->> Only makes sense when used with the above. Knock out Topics containing these words.
// bParial - Partial Word matching is ok - otherwise we match exaclty what is entered
// sTarget - target window/frame -- default = 'content'
// bTitleSearch - Title search only
// ----------------------------------------------
// -- To match similar spellings in a full-text search, select the Match similar words check box.
//    eg  "add", "adds", and "added".
// -- To search for words in document titles only, select the Search titles only check box.
// -- To highlight words in searched topics
//------------------------------------------------------------------------------
//  Notes
//  - DoSearch(s1, s2, s3. partial)
//     S1 is a string of words separated by spaces. Words are OR'd together
//     S2 is a string of words separated by spaces. Words are AND'd together
//     S3 is a string of words separated by spaces. Words are Deleted from other results
//     bParial true = Partial Match OK. false = exact match
//     sTarget window for results - eg "right"
//     bTitleSearch Search Titles only
//  - User can override default properties of S1 and S2 by using the following keywords
//    "OR","|" the next word is OR'd
//    "AND","&","+" the next word is AND'd
//    "NOT","!" the next word is removed
//
//------------------------------------------------------------------------------
function DoSearch(s1, s2, s3, bParial, sTarget, bTitleSearch)
{
  //----------------------------------------------------------------------------
  // Init
  //   - Reset First AND call flag. The first time must be an OR.
  //   - Clear SearchResults list
  //   - Clear target list control
  //----------------------------------------------------------------------------
  gFirstFindCall = true;
  SearchResults.length = 0;
  gFindList.length = 0;
    
  var searchcontainer = document.getElementById("searchcontainer");    
    
  if (searchcontainer)
    searchcontainer.length = 0;
  PARAM_PartialMatchOK = bParial;
  PARAM_TargetWindow = 'content'; //default
  if (sTarget !== null && sTarget !== '') PARAM_TargetWindow = sTarget;
  var searchTitlesOnly = (bTitleSearch == true);
    
  if (!s1 && !s2)
      return 0;
  if (!s1.trim() && !s2.trim())
      return 0;

  //----------------------------------------------------------------------------
  // 3/1/2015 New - Check for full match in title text
  //----------------------------------------------------------------------------
  ProcessTitleSearch(s1, OPT_OR);
  ProcessTitleSearch(s2, OPT_AND);
    
  //----------------------------------------------------------------------------
  //1. (OR) Find documents with "Any of these Words"  ==> SearchResults
  //2. (AND) Find documents with "All these Words"  ==> SearchResults
  //3. (NOT) SearchResults must NOT files containing these words ==> Remove from SearchResults
  //----------------------------------------------------------------------------
  if (!searchTitlesOnly) 
  {
      ProcessSearchTerms(s1, OPT_OR);
      ProcessSearchTerms(s2, OPT_AND);
      ProcessSearchTerms(s3, OPT_NOT);
  }
  
  //----------------------------------------------------------------------------
  // Display SearchResults
  //----------------------------------------------------------------------------
  if (SearchResults.length == 0) {
    alert("No matches found!");
    return(0); }

  //Search Results list exists 
  var test = false;    
  if (!searchcontainer || test) {
    ShowSearchResultsWindow();
  }
  else    
  {
    //Fill SearchResults List -- 500 item limit same as H1.x and H2.x
    for (var i=0;((i<SearchResults.length) && (i<500));i++) {
      var new_option = document.createElement('option');
      new_option.text = SearchTitles[SearchResults[i]];
      new_option.text= new_option.text.replace(/\&amp;/g,'&');
      new_option.value = SearchFiles[SearchResults[i]];
      searchcontainer.options.add(new_option);
    }

    //open the first file
    // ** Comment this line out if you don't want the first Search result displayed automatically ** 
    OpenResultListDoc();
  }

  return(SearchResults.length);
}

//------------------------------------------------------------------------------
// Same as above but returns results. Does not output search results to a list control.
// Same params but not sTarget (since it doesn't display results)
//------------------------------------------------------------------------------
function DoSearchResults(s1, s2, s3, bParial, bTitleSearch)
{
    
  //----------------------------------------------------------------------------
  // Init
  //   - Reset First AND call flag. The first time must be an OR.
  //   - Clear SearchResults list
  //   - Clear target list control
  //----------------------------------------------------------------------------
  gFirstFindCall = true;
  SearchResults.length = 0;
  gFindList.length = 0;

  PARAM_PartialMatchOK = bParial;
  var searchTitlesOnly = (bTitleSearch == true);
  
  if (!s1 && !s2)
      return 0;
  if (!s1.trim() && !s2.trim())
      return 0;

  //----------------------------------------------------------------------------
  // 3/1/2015 New - Check for full match in title text
  //----------------------------------------------------------------------------
  ProcessTitleSearch(s1, OPT_OR);
  ProcessTitleSearch(s2, OPT_AND);
  
  //----------------------------------------------------------------------------
  //1. (OR) Find documents with "Any of these Words"  ==> SearchResults
  //2. (AND) Find documents with "All these Words"  ==> SearchResults
  //3. (NOT) SearchResults must NOT files containing these words ==> Remove from SearchResults
  //----------------------------------------------------------------------------
  if (!searchTitlesOnly) 
  {
      ProcessSearchTerms(s1, OPT_OR);
      ProcessSearchTerms(s2, OPT_AND);
      ProcessSearchTerms(s3, OPT_NOT);
  }
  
  return(SearchResults.slice(0));
}




//----------------------------------------------------------------------------
// OR -- Add only Unique items to the SearchResults Array
//   items - array of Idxs to OR into SearchResults
//   SearchResults - Pass in by ref to to optomize global scope access
//----------------------------------------------------------------------------
function OR_WithSearchResults(items, SearchResults) {
  var found;
  for (var i = 0; i < items.length; i++) {

    //Already in list?
    found = false;
    for (var k = 0; (k < SearchResults.length) && (!found); k++)
      if (items[i] == SearchResults[k])
        found = true;

    //Not in list? Then Add it!
    if (!found)
      SearchResults[SearchResults.length] = items[i];
  }
}

//----------------------------------------------------------------------------
// AND -- Keep only the intersection of items and SearchResults
//   items - array of Idxs to AND into SearchResults
//   SearchResults - Pass in by ref to to optomize global scope access
//----------------------------------------------------------------------------
function AND_WithSearchResults(items, SearchResults) {
  var count = 0;
  for (var i = 0; i < SearchResults.length; i++)
    for (var k = 0; k < items.length; k++) {
      if (items[k] == SearchResults[i]) {
        SearchResults[count] = SearchResults[i];
        count++;
        break;
      }
    }
  SearchResults.length = count;
}

//----------------------------------------------------------------------------
// DEL -- Remove items from SearchResults list
//   items - array of Idxs to DEL from SearchResults
//   SearchResults - Pass in by ref to to optomize global scope access
//----------------------------------------------------------------------------
function DEL_WithSearchResults(items, SearchResults) {
  var count = 0;
  var found; 
  for (var i = 0; i < SearchResults.length; i++) {

    //Its Delete Item in the Result list?
    found = false;
    for (var k = 0; (k < items.length) && (!found); k++)
      if (items[k] == SearchResults[i]) {
        found = true;
        break;
      }

    //Not Found in delete list? then keep Result
    if (!found) {
      SearchResults[count] = SearchResults[i];
      count++;
      }
  }
  SearchResults.length = count;
}


//----------------------------------------------------------------------------
// Find Database Text
// By this stage all strings are Uppercase
// Optimization
//   - String Compare - check length the same, and check first char match before
//     going on to actually do a string compare.
//   - Pass Global SearchIndexes in instead of accessing out of scope many times
//----------------------------------------------------------------------------

//Find Text (in SearchIndex passed in by ref) and OR matches into SearchResults list
function FindText_OR(SrchText, SearchIndexes, SearchResults)
{
  if (PARAM_PartialMatchOK) {
    for(var i=0;i<SearchIndexes.length;i++)
      if((SearchIndexes[i][0].length >= SrchText.length)
      && (SearchIndexes[i][0].indexOf(SrchText) >= 0)) {
        OR_WithSearchResults(SearchIndexes[i][1], SearchResults);
      }
  }
  else {
    //Not Partial - Fast - Find exact match and break out  
    for(var i=0;i<SearchIndexes.length;i++)
      if((SearchIndexes[i][0].length == SrchText.length)
      && (SearchIndexes[i][0] == SrchText)) {
        OR_WithSearchResults(SearchIndexes[i][1], SearchResults);
        break;
      }
  }
}

//Find Text (in SearchIndex passed in by ref) and AND matches into SearchResults list
function FindText_AND(SrchText, SearchIndexes, SearchResults)
{
  //Optimization: Take copy to minimize global out of scope lookups
  var FirstFindCall = gFirstFindCall;

  //If 2nd or 3rd... item and No SearchResults then Nothing to AND with
  if ((!FirstFindCall) && (SearchResults.length == 0))
    return;

  var tempList = [];
  if (PARAM_PartialMatchOK) {
    for(var i=0;i<SearchIndexes.length;i++)
      if((SearchIndexes[i][0].length >= SrchText.length)
      && (SearchIndexes[i][0].indexOf(SrchText) >= 0))
        OR_WithSearchResults(SearchIndexes[i][1], tempList);
  }
  else {
    //Not Partial - Fast - Find exact match and break out
    for(var i=0;i<SearchIndexes.length;i++)
      if((SearchIndexes[i][0].length == SrchText.length)
      && (SearchIndexes[i][0] == SrchText)) {
        OR_WithSearchResults(SearchIndexes[i][1], tempList);
        //Exact match - we are done
        break;
      }
  }

  //Add Results
  //1st call wont have results yet -- We must OR into SearchResults as AND would not do nothing
  if (tempList.length >= 0) {
    if (FirstFindCall)
      OR_WithSearchResults(tempList, SearchResults);
    else
      AND_WithSearchResults(tempList, SearchResults);
  }
  else
    //No Results + not first call -- AND will wipe out all results
    if (!FirstFindCall)
      SearchResults.length = 0;
}


//Find Text (in SearchIndex passed in by ref) and DELETE matches from SearchResults list
function FindText_DEL(SrchText, SearchIndexes, SearchResults)
{
  //first check there is something to delete from
  if (SearchResults.length)
    for(var i=0;i<SearchIndexes.length;i++)
      if((SearchIndexes[i][0].length == SrchText.length)
      &&(SearchIndexes[i][0]==SrchText)) {

        //Send match words idx array off to be deleted from SearchResults
        DEL_WithSearchResults(SearchIndexes[i][1], SearchResults);

        //We found the word and its idx array data -- Jobs done
        break;
      }
}


//------------------------------------------------------------------------------
// Highlight search hits
//------------------------------------------------------------------------------

var targetWin = null;
var highlightStartTag = "<span style='background-color:#FFFF00;color:#222222;'>";  //yellow highlight
var highlightEndTag = "</span>";

//------------------------------------------------------------------------------
// Highlight text by adding HTML tags before and after all occurrences of the search term.
// Acknowledgments: Thank you Sujit Kumar Shah - http://www.sks.com.np/article/14/pure-javascript-search-and-text-highlighting.html
//------------------------------------------------------------------------------
function doHighlight(bodyText, searchTerm) 
{
  var newText = "";
  var i = -1;
  var lcSearchTerm = searchTerm.toLowerCase();
  var lcBodyText = bodyText.toLowerCase();
    
  while (bodyText.length > 0) {
    i = lcBodyText.indexOf(lcSearchTerm, i+1);
    if (i < 0) {
      newText += bodyText;
      bodyText = "";
    } else {
      // skip anything inside an HTML tag
      if (bodyText.lastIndexOf(">", i) >= bodyText.lastIndexOf("<", i)) {
        // skip anything inside a <script> block
        if (lcBodyText.lastIndexOf("/script>", i) >= lcBodyText.lastIndexOf("<script", i)) {
          newText += bodyText.substring(0, i) + highlightStartTag + bodyText.substr(i, searchTerm.length) + highlightEndTag;
          bodyText = bodyText.substr(i + searchTerm.length);
          lcBodyText = bodyText.toLowerCase();
          i = -1;
        }
      }
    }
  }
  return newText;
}

// Highlight search terms
function HighlightTopic() {
  try {
    var bodyText = targetWin.document.body.innerHTML;
    for(var k = 0; k < gFindList.length; k++) {
      bodyText = doHighlight(bodyText, gFindList[k]);
    }
    targetWin.document.body.innerHTML = bodyText;
  }
  catch(err) {  //if interframe security on then there will be silent exception
  }
}

//------------------------------------------------------------------------------
// Open List item in Browser - Target = PARAM_TargetWindow
//------------------------------------------------------------------------------
function OpenResultListDoc() {
    //Something selected in the search result list?
    var searchcontainer = document.getElementById("searchcontainer");  
    if (searchcontainer && searchcontainer.options.length >= 0)
    {
        var iSelect = searchcontainer.selectedIndex;
        if (iSelect < 0) {
            iSelect = 0;
            searchcontainer.selectedIndex = 0;
        }
        
        //Open the selected file
        if (window.navigator.userAgent.indexOf("Netscape") > 0) {
          top.right.location.href = searchcontainer.options[iSelect].value;
          targetWin = top.right.window;
        }
        else { //all other browsers
          targetWin = window.open(searchcontainer.options[iSelect].value, PARAM_TargetWindow); 
          window.focus(); //keep focus
          searchcontainer.focus();
        }

        //Highlight search hits on a delay
        if (targetWin)
          setTimeout('HighlightTopic()',1000);
    }
}


//------------------------------------------------------------------------------
// Search Results Window -- called if user does not
//  -- 500 item limit same as H1.x and H2.x
//------------------------------------------------------------------------------
function ShowSearchResultsWindow() {
  //var newWindow = window.open("about:blank", "searchValue", "width=500, height=300, resizable=yes, maximizable=no, status=yes, scrollbars=yes");
  var newWindow = window.open("about:blank", "_self", "width=500, height=300, resizable=yes, maximizable=no, status=no, scrollbars=no");
  newWindow.document.write('<html>\n<head>\n<title>Search Results</title>\n');
  newWindow.document.write('</head>\n');
  newWindow.document.write('<body style="margin:0px;height:100%;background-color:#f0f8ff;font-size:80%;font-family:arial,verdana;">\n');
  newWindow.document.write('<div style="height:100%;width:100%;padding:0px;margin:0px;overflow:auto;">');  
  newWindow.document.write('<div style="height:25px;width:98%;padding:8px 0px 3px 2%;background-color:#545444;color:#e2e2e2;">Search Results<a onclick="location.reload();" style="float:right;color:#e2e2e2;background-color:#747474;cursor:pointer;padding:2px 5px 0px 5px;margin-right:5px;">Close</a></div>\n');
  newWindow.document.write('<div style="height:100%;width:98%;padding-left:2%">\n');

  //Fill SearchResults List
  newWindow.document.write('<p>Results: ' + SearchResults.length + '</p>\n');
  for(var i=0;((i<SearchResults.length) && (i<500));i++) {
    //Search Topic Title
    var aTitle = SearchTitles[SearchResults[i]];
    //URL
    var aURL = SearchFiles[SearchResults[i]];

    newWindow.document.write('<p>Title: '+ aTitle +'<br>\n');
    newWindow.document.write('URL: <a href="'+ aURL +'" target="'+PARAM_TargetWindow+'">'+aURL+'</a></p>\n');
  }

  newWindow.document.write('</div>\n');
  newWindow.document.write('</div>\n');
  newWindow.document.write("</body>\n");
  newWindow.document.write("</html>\n");
  newWindow.document.close();
//  self.name = "main";
}

//------------------------------------------------------------------------------
// Other Script
//------------------------------------------------------------------------------
function CloseNavPane() {
  try {
    if ((top.content.location == null) || (top.content.location == undefined) || (typeof(top.content.location.href) != "string") || (top.content.location.href == ""))
      top.location="index.htm";   //can't work out the current content file - return home
    else
      top.location=top.content.location;
  }
  catch(err) {
    top.location="index.htm";   //default to home page
  }
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------




