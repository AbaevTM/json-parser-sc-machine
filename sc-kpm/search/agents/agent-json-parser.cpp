/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_semantic_neighborhood.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>
#include <glib.h>

#include <rapidjson/reader.h>
#include <iostream>

using namespace rapidjson;
using namespace std;
struct MyHandler {
    bool RawNumber(const char* str, SizeType length, bool copy) { cout << "Null()" << endl; return true; }
    bool Null() { cout << "Null()" << endl; return true; }
    bool Bool(bool b) { g_print("BOOL!!!\n"); return true; }
    bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
    bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
    bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
    bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
    bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
    bool String(const char* str, SizeType length, bool copy) {
        cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool StartObject() { cout << "StartObject()" << endl; return true; }
    bool Key(const char* str, SizeType length, bool copy) {
        cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
    bool StartArray() { cout << "StartArray()" << endl; return true; }
    bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
};

/*!
 * Function that implements sc-agent of parsing json contents of nodes contained in json_format set.
 */
extern "C" sc_result agent_json_parser(const sc_event *event, sc_addr arg) {
    g_print("WOOOOOOOOOOOOOOOOOOOOOOOOOOOOR!!!\n");
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    MyHandler handler;
    Reader reader;
    StringStream ss(json);
    reader.Parse(ss, handler);
    return SC_RESULT_ERROR;
}




