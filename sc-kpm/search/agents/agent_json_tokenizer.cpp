/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */
extern "C"{
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>
}

#include <rapidjson/reader.h>
#include <iostream>

const char * AGENT_LOG_PREFIX = "C_AGENT JSON TOKENIZER : ";

using namespace rapidjson;
using namespace std;
struct MyHandler {
    bool RawNumber(const char* str, SizeType length, bool copy) { cout << "Null()" << endl; return true; }
    bool Null() { cout << "Null()" << endl; return true; }
    bool Bool(bool b) { cout << "!231231231BOOL!!!\n"; return true; }
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

sc_result checkArgumentTypes(sc_addr arc, sc_addr * node) {
    sc_result result = SC_RESULT_ERROR_INVALID_PARAMS;
    sc_type arc_type;
    sc_type node_type;
    if (sc_memory_get_element_type(s_default_ctx, arc, &arc_type) == SC_RESULT_OK
            && arc_type == sc_type_arc_pos_const_perm) {
        cout << AGENT_LOG_PREFIX <<" arc's type : " << arc_type;
        if (sc_memory_get_arc_end(s_default_ctx, arc, node) == SC_RESULT_OK) {
            if (sc_memory_get_element_type(s_default_ctx, *node, &node_type) == SC_RESULT_OK
                    && node_type & sc_type_link) {
                result = SC_RESULT_OK;
            }
            else {
                cout << AGENT_LOG_PREFIX << " arc's end element is not a link. Type : " << node_type << "\n";
            }
        } else {
            cout << AGENT_LOG_PREFIX << " cannot get arc's end node containing JSON\n";
        }
    } else {
        cout << AGENT_LOG_PREFIX << " element triggered event is not an arc of sc_type_arc_pos_const_perm type\n";
    }
    return result;
}

void readNodeContent(sc_char **data, sc_addr idtf) {
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_memory_get_link_content(s_default_ctx, idtf, &stream);
    sc_stream_get_length(stream, &length);
    (*data) = (sc_char*) calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, *data, length, &read_length);
    (*data)[length] = '\0';
    cout << AGENT_LOG_PREFIX << "\nNode content : ";
    for (int index = 0; index < length; index++) {
        cout << (*data)[index];
    }
    cout << "\n";
    sc_stream_free(stream);
}

/*!
 * Function that implements sc-agent of parsing json contents of nodes contained in json_format set.
 */
extern "C" sc_result agent_json_tokenizer(const sc_event *event, sc_addr arg) {
    cout << AGENT_LOG_PREFIX << "starting\n";
    sc_addr node;
    if(checkArgumentTypes(arg, &node) == SC_RESULT_OK) {
        //const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
        MyHandler handler;
        Reader reader;
        sc_char *data;
        readNodeContent(&data, node);
        StringStream ss(data);
        reader.Parse(ss, handler);
        free(data);    
    } 
    cout << AGENT_LOG_PREFIX << "ending\n";
    return SC_RESULT_OK;
}




