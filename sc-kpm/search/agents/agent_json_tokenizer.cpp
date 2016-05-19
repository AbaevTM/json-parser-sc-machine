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

struct EmptyJSONVerifyingHandler {
    bool RawNumber(const char* str, SizeType length, bool copy) {
        return true;
    }
    bool Null() {
        return true;
    }
    bool Bool(bool b) {
        return true;
    }
    bool Int(int i) {
        return true;
    }
    bool Uint(unsigned u) {
        return true;
    }
    bool Int64(int64_t i) {
        return true;
    }
    bool Uint64(uint64_t u) {
        return true;
    }
    bool Double(double d) {
        return true;
    }
    bool String(const char* str, SizeType length, bool copy) {
        return true;
    }
    bool StartObject() {
        return true;
    }
    bool Key(const char* str, SizeType length, bool copy) {
        return true;
    }
    bool EndObject(SizeType memberCount) {
        return true;
    }
    bool StartArray() {
        return true;
    }
    bool EndArray(SizeType elementCount) {
        return true;
    }
};
struct JSONTokenizingHandler {
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

  ///////////////////////////////
 ///// CONSTANTS ///////////////
///////////////////////////////

#define FORMAT_JSON_WELL_FORMATTED      "format_json_well_formatted"

#define FORMAT_JSON_MISFORMATTED        "format_json_misformatted"

sc_addr format_json_well_formatted;

sc_addr format_json_misformatted;

sc_result initializeConstants() {
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                FORMAT_JSON_WELL_FORMATTED,
                strlen(FORMAT_JSON_WELL_FORMATTED),
                &format_json_well_formatted
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << FORMAT_JSON_MISFORMATTED << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                FORMAT_JSON_MISFORMATTED,
                strlen(FORMAT_JSON_MISFORMATTED),
                &format_json_misformatted
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << FORMAT_JSON_MISFORMATTED << "\n";
        return SC_RESULT_ERROR;
    }
    return SC_RESULT_OK;
}

sc_result checkArgumentTypes(sc_addr arc, sc_addr * node) {
    sc_result result = SC_RESULT_ERROR_INVALID_PARAMS;
    sc_type arc_type;
    sc_type node_type;
    if (sc_memory_get_element_type(s_default_ctx, arc, &arc_type) == SC_RESULT_OK
            && arc_type == sc_type_arc_pos_const_perm) {
        cout << AGENT_LOG_PREFIX <<" arc's type : " << arc_type << "\n";
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

void removeValidationMarks(sc_addr node) {
    sc_iterator3 * validArcsIterator = sc_iterator3_f_a_f_new(
        s_default_ctx,
        format_json_misformatted,
        sc_type_arc_pos_const_perm,
        node
    );
    sc_addr arcToRemove;
    while(sc_iterator3_next(validArcsIterator) == SC_TRUE) {
        arcToRemove = sc_iterator3_value(validArcsIterator, 1);
        sc_memory_element_free(s_default_ctx, arcToRemove);
    }
    sc_iterator3_free(validArcsIterator);
    validArcsIterator = sc_iterator3_f_a_f_new(
            s_default_ctx,
            format_json_well_formatted,
            sc_type_arc_pos_const_perm,
            node
        );
    while(sc_iterator3_next(validArcsIterator) == SC_TRUE) {
        arcToRemove = sc_iterator3_value(validArcsIterator, 1);
        sc_memory_element_free(s_default_ctx, arcToRemove);
    }
    sc_iterator3_free(validArcsIterator);
}

/*!
 * Function that implements sc-agent of parsing json contents of nodes contained in json_format set.
 */
extern "C" sc_result agent_json_tokenizer(const sc_event *event, sc_addr arg) {
    cout << "***************\n" << AGENT_LOG_PREFIX << "starting\n";
    sc_addr node;
    if(initializeConstants() == SC_RESULT_OK
            && checkArgumentTypes(arg, &node) == SC_RESULT_OK) {
        EmptyJSONVerifyingHandler verifyingHandler;
        Reader verifyingReader;
        sc_char *data;
        readNodeContent(&data, node);
        StringStream verifyingStream(data);
        removeValidationMarks(node);
        bool isJSONValid = verifyingReader.Parse(verifyingStream, verifyingHandler);
        if (isJSONValid) {
            JSONTokenizingHandler handler;
            Reader reader;
            StringStream jsonStream(data);
            reader.Parse(jsonStream, handler);
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, format_json_well_formatted, node);
        } else {
            cout << AGENT_LOG_PREFIX << " marking as misformatted node : " << node.seg << "|" << node.offset << "\n";
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, format_json_misformatted, node);
        }
        free(data);    
    } 
    cout << AGENT_LOG_PREFIX << "ending\n***************\n";
    return SC_RESULT_OK;
}




