/**
 *  @file stringUtils.cpp
 *
 */
/*
 * $Id$
 */
// Copyright 2001  California Institute of Technology

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#pragma warning(disable:4996)
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

#include "ct_defs.h"
#include "stringUtils.h"
#include "ctexceptions.h"
#include "global.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "ctml.h"

#include <string>

namespace Cantera {


  /**
   * Convert a floating point number to a std::string using sprintf.
   */
  std::string fp2str(double x, std::string fmt) {
    char buf[64];
    int n = SNPRINTF(buf, 63, fmt.c_str(), x);
    if (n > 0) {
      buf[63] = '\0';
      return std::string(buf);
    } 
    return std::string(" ");
  }
  std::string fp2str(double x) {
    char buf[64];
    int n = SNPRINTF(buf, 64, "%g" , x);
    if (n > 0) {
      buf[29] = '\0';
      return std::string(buf);
    } 
    return std::string(" ");
  }

  /**
   * Convert an integer number to a std::string using sprintf.
   */
  std::string int2str(int n, std::string fmt) {
    char buf[30];
    int m = SNPRINTF(buf, 30, fmt.c_str(), n);
    //sprintf(buf, fmt.c_str(), n);
    if (m > 0) {
      buf[29] = '\0';
      return std::string(buf);
    }
    return std::string(" ");
  }

  
  std::string int2str(int n) {
    char buf[30];
    int m = SNPRINTF(buf, 30, "%d", n);
    if (m > 0) {
      buf[29] = '\0';
      return std::string(buf);
    }
    return std::string(" ");
  }

  std::string lowercase(const std::string &s) {
    int n = static_cast<int>(s.size());
    std::string lc(s);
    for (int i = 0; i < n; i++) lc[i] = tolower(s[i]);
    return lc;
  }

  static int firstChar(std::string s) {
    int i;
    int n = static_cast<int>(s.size());
    for (i = 0; i < n; i++) 
      if (s[i] != ' ' && isprint(s[i])) break;
    return i;
  }

  static int lastChar(std::string s) {
    int i;
    int n = static_cast<int>(s.size());
    for (i = n-1; i >= 0; i--) 
      if (s[i] != ' ' && isprint(s[i])) break;
    return i;
  }

  /** 
   * Strip leading and trailing white space.
   */
  std::string stripws(std::string s) {
    int ifirst = firstChar(s);
    int ilast = lastChar(s);
    return s.substr(ifirst, ilast - ifirst + 1); 
  }

  /** 
   * Strip non-printing characters.
   */
  std::string stripnonprint(std::string s) {
    int i;
    int n = static_cast<int>(s.size());
    std::string ss = "";
    for (i = 0; i < n; i++) {
      if (isprint(s[i])) {
	ss += s[i];
      }
    }
    return ss;
  }

            
  /**
   * Parse a composition string.
   */
  void parseCompString(const std::string ss, compositionMap& x) {
    std::string s = ss;
    std::string::size_type icolon, ibegin, iend;
    std::string name, num, nm;
    do {
      ibegin = s.find_first_not_of(", ;\n\t");
      if (ibegin != std::string::npos) {
	s = s.substr(ibegin,s.size());
	icolon = s.find(':');
	iend = s.find_first_of(", ;\n\t");
	//icomma = s.find(',');
	if (icolon != std::string::npos) {
	  name = s.substr(0, icolon);
	  if (iend != std::string::npos) {
	    num = s.substr(icolon+1, iend-icolon);
	    s = s.substr(iend+1, s.size());
	  }
	  else {
	    num = s.substr(icolon+1, s.size());
	    s = "";
	  }
	  nm = stripws(name);
	  if (x.find(nm) == x.end()) {
	    //if (x[nm] == 0.0) {
	    throw CanteraError("parseCompString",
			       "unknown species " + nm);
	  }
	  x[nm] = atof(num.c_str());
	}
	else s = "";
      }
    }
    while (s != "");
  }


            
  /**
   * Parse a composition string.
   */
  void split(const std::string ss, std::vector<std::string>& w) {
    std::string s = ss;
    std::string::size_type ibegin, iend;
    std::string name, num, nm;
    do {
      ibegin = s.find_first_not_of(", ;\n\t");
      if (ibegin != std::string::npos) {
	s = s.substr(ibegin,s.size());
	iend = s.find_first_of(", ;\n\t");
	if (iend != std::string::npos) {
	  w.push_back(s.substr(0, iend));
	  s = s.substr(iend+1, s.size());
	}
	else {
	  w.push_back(s.substr(0, s.size()));
	  return;
	}
      }
    }
    while (s != "");
  }

  int fillArrayFromString(const std::string& str, doublereal* a, char delim) {
    std::string::size_type iloc;
    int count = 0;
    std::string num, s;
    s = str;
    while (s.size() > 0) {
      iloc = s.find(delim);
      if (iloc > 0) {
	num = s.substr(0, iloc);
	s = s.substr(iloc+1,s.size());
      }
      else {
	num = s;
	s = "";
      }
      a[count] = atof(num.c_str());
      count++;
    }
    return count;
  }

  /**
   *  Get the file name without the path or extension
   */
  std::string getFileName(const std::string& path) {
    std::string file;
    size_t idot = path.find_last_of('.');
    size_t islash = path.find_last_of('/');
    if (idot > 0 && idot < path.size()) {
      if (islash > 0 && islash < idot) {
	file = path.substr(islash+1, idot-islash-1);
      }
      else {
	file = path.substr(0,idot);
      }
    }
    else {
      file = path;
    }       
    return file;
  }

  
  int intValue(std::string val) {
    return std::atoi(stripws(val).c_str());
  }

  doublereal fpValue(std::string val) {
    return std::atof(stripws(val).c_str());
  }

  doublereal fpValueCheck(std::string val) {
    return atofCheck(stripws(val).c_str());
  }

  /**
   *  Generate a logfile name based on an input file name
   */
  std::string logfileName(const std::string& infile) {
    std::string logfile;
    size_t idot = infile.find_last_of('.');
    size_t islash = infile.find_last_of('/');
    if (idot > 0 && idot < infile.size()) {
      if (islash > 0 && islash < idot) {
	logfile = infile.substr(islash+1, idot-islash-1) + ".log";
      }
      else {
	logfile = infile.substr(0,idot) + ".log";
      }
    }
    else {
      logfile = infile + ".log";
    }       
    return logfile;
  }

  std::string wrapString(const std::string& s, int len) {
    int nc = s.size();
    int n, count=0;
    std::string r;
    for (n = 0; n < nc; n++) {
      if (s[n] == '\n') count = 0;
      else count++;
      if (count > len && s[n] == ' ') {
	r += "\n     ";
	count = 0;
      }
      r += s[n];
    }
    return r;
  }


  /*
   *     This routine strips off blanks and tabs (only leading and trailing
   *     characters) in 'str'.  On return, it returns the number of
   *     characters still included in the string (excluding the null character).
   *
   *      Comments are excluded -> All instances of the comment character, '!',
   *                               are replaced by '\0' thereby terminating
   *                               the string
   *
   *     Parameter list:
   *
   *     str == On output 'str' contains the same characters as on
   *               input except the leading and trailing white space and
   *               comments have been removed.
   */
  int stripLTWScstring(char str[]) {
    int  i = 0, j = 0;
    char ch;
    const char COM_CHAR='\0';
    /*
     *    Quick Returns
     */
    if ((str == 0) || (str[0] == '\0')) return (0);

    /* Find first non-space character character */
    while(((ch = str[i]) != '\0') && isspace(ch)) i++;
	
    /*
     * Move real part of str to the front by copying the string
     *   - Comments are handled here, by terminating the copy at the
     *     first comment indicator, and inserting the null character at
     *     that point.
     */
 
    while ( (ch = str[j+i]) != '\0' &&
	    (ch != COM_CHAR)) {
      str[j] = ch;
      j++;
    }
    str[j] = '\0';
    j--;
    /* Remove trailing white space by inserting a null character */    
    while( (j != -1 ) && isspace(str[j])) j--;
    j++;
    str[j] = '\0';
    return (j);
  }


  /*
   * atofCheck is a wrapper around the C stdlib routine atof().
   * It does quite a bit more error checking than atof() or
   * strtod(), and is quite a bit more restrictive.
   *
   *   First it interprets both E, e, d, and D as exponents.
   *   atof() only interprets e or E as an exponent character.
   *
   *   It only accepts a string as well formed if it consists as a 
   *   single token. Multiple words will produce an error message
   *
   *   It will produce an error for NAN and inf entries as well,
   *   in contrast to atof() or strtod().
   *   The user needs to know that a serious numerical issue
   *   has occurred.
   *
   *   It does not accept hexadecimal numbers.
   *
   * On any error, it will throw a CanteraError signal.
   */
  double atofCheck(const char *dptr) {
    if (!dptr) {
      throw CanteraError("atofCheck", "null pointer to string");
    }
    char *eptr = (char *) malloc(strlen(dptr)+1);
    strcpy(eptr, dptr);
    int ll = stripLTWScstring(eptr);
    if (ll == 0) {
      throw CanteraError("atofCheck", "string has zero length");
    }
    int numDot = 0;
    int numExp = 0;
    char ch;
    int istart = 0;
    ch = eptr[0];
    if (ch == '+' || ch == '-') {
      istart = 1;
    }
    for (int i = istart; i < ll; i++) {
      ch = eptr[i];
      if (isdigit(ch)) {
      } else if (ch == '.') {
	numDot++;
	if (numDot > 1) {
	  free(eptr);
	  throw CanteraError("atofCheck",
			     "string has more than one .");
	}
      } else if (ch == 'e' || ch == 'E' || ch == 'd' || ch == 'D') {
	numExp++;
	eptr[i] = 'E';
	if (numExp > 1) {
	  free(eptr);
	  throw CanteraError("atofCheck", 
			     "string has more than one exp char");
	}
	ch = eptr[i+1];
	if (ch == '+' || ch == '-') {
	  i++;
	}
      } else {
	std::string hh(dptr);
	free(eptr);
	throw CanteraError("atofCheck",
			   "Trouble processing string, " + hh);
      }
    }
    double rval = atof(eptr);
    free(eptr);
    return rval;
  }


  doublereal strSItoDbl(const std::string& strSI) {
    std::vector<std::string> v;
    tokenizeString(strSI, v);
    doublereal fp = 1.0;
    int n = v.size();
    if (n > 2 || n < 1) {
      throw CanteraError("strSItoDbl",
			 "number of tokens is too high");
    } else if (n == 2) {
      fp = toSI(v[1]);
    }
    doublereal val = atofCheck(v[0].c_str());
    return (val * fp);
  }

  static std::string::size_type findFirstWS(const std::string& val) {
    std::string::size_type ibegin = std::string::npos;
    int j = 0;
    std::string::const_iterator i = val.begin();
    for ( ; i != val.end(); i++) {
      char ch = *i;
      int ll = (int) ch;
      if (isspace(ll)) {
	ibegin = (std::string::size_type) j;
	break;
      }
      j++;
    }
    return ibegin;  
  }

  static std::string::size_type findFirstNotOfWS(const std::string& val) {
    std::string::size_type ibegin = std::string::npos;
    int j = 0;
    std::string::const_iterator i = val.begin();
    for ( ; i != val.end(); i++) {
      char ch = *i;
      int ll = (int) ch;
      if (!isspace(ll)) {
	ibegin = (std::string::size_type) j;
	break;
      }
      j++;
    }
    return ibegin;  
  }

  // This function  separates a string up into tokens
  // according to the location of white space.
  /*
   *    The separate tokens are returned in a string vector, v.
   *
   *  @param oval   String to be broken up
   *  @param v     Output vector of tokens.
   */
  void tokenizeString(const std::string& oval,
                      std::vector<std::string>& v) {
    std::string val(oval);
    std::string::size_type ibegin, iend;
    v.clear();
    while (1 > 0) {
      ibegin = findFirstNotOfWS(val);
      if (ibegin != std::string::npos) {
        val = val.substr(ibegin,val.size());
        //iend = val.find_first_of(" \n\t");
        iend = findFirstWS(val);
        if (iend == std::string::npos) {
          v.push_back(val);
          break;
        } else {
          v.push_back(val.substr(0,iend));
          val = val.substr(iend+1,val.size());
        }
      }
      else {
        break;
      }
    }
  }


}
