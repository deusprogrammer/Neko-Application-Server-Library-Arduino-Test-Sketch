#include "strutils.h"

#ifndef SIMHTTP3_H
#define SIMHTTP3_H

#ifndef NULL
#define NULL 0
#endif

enum HTTPVerb {GET, PUT, POST, DELETE};

class HTTPQueryPair {
private:
	char element[32], value[128];
public:
	HTTPQueryPair() {}
	HTTPQueryPair(char* element, char* value) {stringCopy(this->element, element); stringCopy(this->value, value);}
	char* getKey() {return element;}
	char* getValue() {return value;}
};

class HTTPQueryString {
private:
	HTTPQueryPair* pairs[64];
	int nPairs;
public:
	HTTPQueryString() {nPairs = 0;}
	~HTTPQueryString() {for (int i = 0; i < nPairs; i++) delete pairs[i];}
	void add(char* element, char* value) {if (nPairs < 64) pairs[nPairs++] = new HTTPQueryPair(element, value);}
	char* operator[](char* key);
};

char* HTTPQueryString::operator[](char* key) {
	for (int i = 0; i < nPairs; i++) {
		if (stringEquals(pairs[i]->getKey(), key))
			return pairs[i]->getValue();
	}

	return NULL;
}

class HTTPRequest {
private:
	HTTPVerb verb;
	HTTPQueryString queryString;
	char resource[1024], httpVersion[64];
public:
	HTTPRequest(char** tokens, int nTokens);
	HTTPRequest(char* line);

	HTTPVerb getVerb() {return verb;}
	HTTPQueryString getQueryString() {return queryString;}
	char* getResource() {return resource;}
	char* getHTTPVersion() {return httpVersion;}
};

HTTPRequest::HTTPRequest(char** tokens, int nTokens) {
	if (nTokens < 3)
		return;

	if (stringEquals(tokens[0], "GET"))
		verb = GET;
	else if (stringEquals(tokens[0], "PUT"))
		verb = PUT;
	else if (stringEquals(tokens[0], "POST"))
		verb = POST;
	else if(stringEquals(tokens[0], "DELETE"))
		verb = DELETE;

	if (stringContains(tokens[1], '?')) {
		int nQTokens;
		char** qTokens = stringSplit(tokens[1], "?&", &nQTokens);

		for (int i = 0; i < nQTokens; i++) {
			if (stringContains(qTokens[i], '=')) {
				int nElements;
				char** qElements = stringSplit(qTokens[i], "=", &nElements);

				if (nElements == 2) {
					queryString.add(qElements[0], qElements[1]);
				}

				delete qElements;
			}
		}

		delete qTokens;
	}

	stringCopy(resource, tokens[1]);
	stringCopy(httpVersion, tokens[2]);
}

class HTTPHeader {
private:
	HTTPRequest* httpRequest;
	char contentLengthStr[64];
	unsigned int contentLength;
public:
	HTTPHeader() {contentLength = 0; httpRequest = NULL;}
	~HTTPHeader() {delete httpRequest;}
	void consumeLine(char* line);
	int getContentLength() {return contentLength;}
	HTTPRequest* getRequest() {return httpRequest;}
};

void HTTPHeader::consumeLine(char* line) {
	int nTokens;
	char** tokens = stringSplit(line, " :", &nTokens);
	
	if (stringEquals(tokens[0], "GET") || stringEquals(tokens[0], "PUT") || stringEquals(tokens[0], "POST") || stringEquals(tokens[0], "DELETE")) {
		httpRequest = new HTTPRequest(tokens, nTokens);
	}
	else if (stringEquals(tokens[0], "Content-Length")) {
		stringCopy(contentLengthStr, tokens[1]);
		contentLength = stringToInt(contentLengthStr);
	}
	//More header items to come

	delete tokens;
}

#endif