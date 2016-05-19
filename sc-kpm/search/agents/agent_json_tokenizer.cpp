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


///////////////////////////////
///// CONSTANTS ///////////////
///////////////////////////////

#define FORMAT_JSON_WELL_FORMATTED      "format_json_well_formatted"

#define FORMAT_JSON_MISFORMATTED        "format_json_misformatted"

#define JSON_TOKEN_LEFT_SQUARE_BRACKET  "json_token_left_square_bracket"

#define JSON_TOKEN_RIGHT_SQUARE_BRACKET  "json_token_right_square_bracket"

#define JSON_TOKEN_LEFT_CURLY_BRACKET  "json_token_left_curly_bracket"

#define JSON_TOKEN_RIGHT_CURLY_BRACKET  "json_token_right_curly_bracket"

#define JSON_TOKEN_COLON  "json_token_colon"

#define JSON_TOKEN_COMMA  "json_token_comma"

#define JSON_TOKEN_TRUE  "json_token_true"

#define JSON_TOKEN_FALSE  "json_token_false"

#define JSON_TOKEN_NULL  "json_token_null"

#define JSON_TOKEN_STRING  "json_token_string"

#define JSON_TOKEN_NUMERIC  "json_token_numeric"

#define JSON_TOKEN_LIST  "json_token_list"

#define JSON_TOKEN_LIST_ITEM  "json_token_list_item"

#define NREL_JSON_TOKEN_LIST_ITEM_VALUE  "nrel_json_token_list_item_value"

#define NREL_JSON_TOKEN_LIST_ITEM_NEXT  "nrel_json_token_list_item_next"

#define RREL_FIRST_JSON_TOKEN_IN_LIST  "rrel_first_json_token_in_list"

#define JSON_TOKEN  "json_token"

#define NREL_TOKENIZED_JSON  "nrel_tokenized_json"

sc_addr format_json_well_formatted;

sc_addr format_json_misformatted;

sc_addr json_token_left_square_bracket;

sc_addr json_token_right_square_bracket;

sc_addr json_token_left_curly_bracket;

sc_addr json_token_right_curly_bracket;

sc_addr json_token_colon;

sc_addr json_token_comma;

sc_addr json_token_true;

sc_addr json_token_false;

sc_addr json_token_null;

sc_addr json_token_string;

sc_addr json_token_numeric;

sc_addr json_token_list;

sc_addr json_token_list_item;

sc_addr nrel_json_token_list_item_value;

sc_addr nrel_json_token_list_item_next;

sc_addr rrel_first_json_token_in_list;

sc_addr json_token;

sc_addr nrel_tokenized_json;

using namespace rapidjson;

using namespace std;

#define ARC_TYPE sc_type_arc_common | sc_type_const

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

    sc_addr first_token_list_tuple;

    sc_addr current_token_list_tuple;

    sc_addr json_formatted_link;

    sc_addr json_token_list;

    bool hasElements = false;

    void initialize() {
        json_token_list = sc_memory_node_new(s_default_ctx, sc_type_node & sc_type_const);
    }

    void connectToJSONFormattedNode() {
        sc_addr tuple = sc_memory_arc_new(s_default_ctx, ARC_TYPE, json_formatted_link, json_token_list);
        sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, nrel_tokenized_json, tuple);
    }

    void createJSONTokenListItem(sc_addr jsonToken) {
        sc_addr new_list_item = sc_memory_node_new(s_default_ctx, sc_type_node & sc_type_const);
        sc_addr arc = sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, json_token_list, new_list_item);
        sc_addr tuple_value = sc_memory_arc_new(s_default_ctx, ARC_TYPE, new_list_item, jsonToken);
        sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, nrel_json_token_list_item_value, tuple_value);
        sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, json_token_list, tuple_value);
        sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, json_token_list, jsonToken);
        if (hasElements) {
            // generate next value
            sc_addr tuple_next = sc_memory_arc_new(s_default_ctx, ARC_TYPE, current_token_list_tuple, new_list_item);
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, nrel_json_token_list_item_next, tuple_next);
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, json_token_list, tuple_next);
        } else {
            hasElements = true;
            first_token_list_tuple = new_list_item;
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, rrel_first_json_token_in_list, arc);
        }
        current_token_list_tuple = new_list_item;
    }

    bool RawNumber(const char* str, SizeType length, bool copy) { cout << "Null()" << endl; return true; }
    bool Null() {
        createJSONTokenListItem(json_token_null);
        return true;
    }
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
    bool StartObject() {
        createJSONTokenListItem(json_token_left_curly_bracket);
        return true;
    }
    bool Key(const char* str, SizeType length, bool copy) {
        cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
        return true;
    }
    bool EndObject(SizeType memberCount) {
        createJSONTokenListItem(json_token_right_curly_bracket);
        return true;
    }
    bool StartArray() {
        createJSONTokenListItem(json_token_left_square_bracket);
        return true;
    }
    bool EndArray(SizeType elementCount) {
        createJSONTokenListItem(json_token_right_square_bracket);
        return true;
    }
};

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
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_LEFT_CURLY_BRACKET,
                strlen(JSON_TOKEN_LEFT_CURLY_BRACKET),
                &json_token_left_curly_bracket
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_LEFT_CURLY_BRACKET << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_RIGHT_CURLY_BRACKET,
                strlen(JSON_TOKEN_RIGHT_CURLY_BRACKET),
                &json_token_right_curly_bracket
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_RIGHT_CURLY_BRACKET << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_LEFT_SQUARE_BRACKET,
                strlen(JSON_TOKEN_LEFT_SQUARE_BRACKET),
                &json_token_left_square_bracket
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_LEFT_SQUARE_BRACKET << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_RIGHT_SQUARE_BRACKET,
                strlen(JSON_TOKEN_RIGHT_SQUARE_BRACKET),
                &json_token_right_square_bracket
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_RIGHT_SQUARE_BRACKET << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_COLON,
                strlen(JSON_TOKEN_COLON),
                &json_token_colon
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_COLON << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_COMMA,
                strlen(JSON_TOKEN_COMMA),
                &json_token_comma
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_COMMA << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_STRING,
                strlen(JSON_TOKEN_STRING),
                &json_token_string
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_STRING << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_NUMERIC,
                strlen(JSON_TOKEN_NUMERIC),
                &json_token_numeric
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_NUMERIC << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_LIST,
                strlen(JSON_TOKEN_LIST),
                &json_token_list
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_LIST << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN_LIST_ITEM,
                strlen(JSON_TOKEN_LIST_ITEM),
                &json_token_list_item
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN_LIST_ITEM << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                NREL_JSON_TOKEN_LIST_ITEM_NEXT,
                strlen(NREL_JSON_TOKEN_LIST_ITEM_NEXT),
                &nrel_json_token_list_item_next
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << NREL_JSON_TOKEN_LIST_ITEM_NEXT << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                NREL_JSON_TOKEN_LIST_ITEM_VALUE,
                strlen(NREL_JSON_TOKEN_LIST_ITEM_VALUE),
                &nrel_json_token_list_item_value
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << NREL_JSON_TOKEN_LIST_ITEM_VALUE << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                RREL_FIRST_JSON_TOKEN_IN_LIST,
                strlen(RREL_FIRST_JSON_TOKEN_IN_LIST),
                &rrel_first_json_token_in_list
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << RREL_FIRST_JSON_TOKEN_IN_LIST << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                JSON_TOKEN,
                strlen(JSON_TOKEN),
                &json_token
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << JSON_TOKEN << "\n";
        return SC_RESULT_ERROR;
    }
    if(sc_helper_find_element_by_system_identifier(s_default_ctx,
                NREL_TOKENIZED_JSON,
                strlen(NREL_TOKENIZED_JSON),
                &nrel_tokenized_json
                ) != SC_RESULT_OK) {
        cout << AGENT_LOG_PREFIX << " cannot find keynode " << NREL_TOKENIZED_JSON << "\n";
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
            handler.initialize();
            reader.Parse(jsonStream, handler);
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, format_json_well_formatted, node);
            handler.json_formatted_link = node;
            handler.connectToJSONFormattedNode();
        } else {
            cout << AGENT_LOG_PREFIX << " marking as misformatted node : " << node.seg << "|" << node.offset << "\n";
            sc_memory_arc_new(s_default_ctx, sc_type_arc_pos_const_perm, format_json_misformatted, node);
        }
        free(data);    
    } 
    cout << AGENT_LOG_PREFIX << "ending\n***************\n";
    return SC_RESULT_OK;
}




