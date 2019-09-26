//g++ -g one.cpp && ./a.out 

#include<iostream>
#include<cstring>

using namespace std;

/*
Input for the Module

1) select * from table where field1=value (variations of query including LIKE %)
2) insert (val1, field1) into table  where field1=value (variations of query including LIKE %)

The above needs to be processed and a new structure created.
queryType :"find" or "insert",
OutputfieldList: "pattern like *" , or filed names
entity: <table name>
conditional expression list:
       in fieldList : value field list store them as key value pairs.

This has to be sent to a separate API. You can just show the above data structure. Put the project in github and share the link.
*/


class Query_Engine
{
	void process_conditions( char const * const conditions_list )
	{
		cout << "Conditions: " << conditions_list << endl;
//separate FSM is required to process conditions
	}

	void process_select_query( char const * const input_query )
	{
		char display_fields[100], entity[100], conditions_list[100];
//improvement required
//if where clause is missing following logic will not work
//sscanf should be replaced with FSM
		sscanf( input_query, "select %s from %s where %s", display_fields, entity, conditions_list );
		cout << "......Processing SELECT query......" << endl;
		cout << "Fields to be displayed: " << display_fields << endl;
		cout << "Entity Name: " << entity << endl;
		process_conditions( conditions_list );
		cout << endl;
	}

	void process_insert_query( char const * const input_query )
	{
		char input_fields[100], entity[100], conditions_list[100];
//improvement required
//if where clause is missing following logic will not work
//sscanf should be replaced with FSM
		sscanf( input_query, "insert %s into %s where %s", input_fields, entity, conditions_list );
		cout << "......Processing INSERT query......" << endl;
		cout << "Fields and values to be inserted: " << input_fields << endl;
		cout << "Entity Name: " << entity << endl;
		process_conditions( conditions_list );
		cout << endl;
	}

public:
	void process_query( char const * const input_query )
	{
		if( strncmp( "select", input_query, 6 ) == 0 )
		{
			process_select_query( input_query );
		}
		else if( strncmp( "insert", input_query, 6 ) == 0 )
		{
			process_insert_query( input_query );
		}
		else
		{
			cerr << endl << "Unknown Query type." << endl;
		}
	}
};

int main()
{
	Query_Engine obj;
	obj.process_query( "select * from table where field1=value" );
	obj.process_query( "insert (val1,field1) into table  where field1=value" );
	return 0;
}
