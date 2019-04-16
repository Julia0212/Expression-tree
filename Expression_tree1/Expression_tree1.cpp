#include "pch.h"
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#pragma warning(disable : 4996);
using namespace std;

struct Node {
	string info;
	int id;
	Node *left;
	Node *right;
};

void evaluating_tokens(string *tokens, string s, int &index_tokens);
bool isParenthesis(string token);
bool isOperator(string token);
Node *infixToRPN(string *tokens, int size);
int priority(string token);
Node *makeNode(string info);
void bst_print_dot(Node* tree, FILE* stream);
void bst_print_dot_aux(Node* node, FILE* stream);
void bst_print_dot_null(Node *key, int nullcount, FILE* stream);
void attachOperator(stack<Node*> &treenodes, stack<Node*> &operators);
bool isFunction(string token);
bool isLeftAssociative(string token);
double evaluateTree(Node *treenode, double x1, double y1, double z1);
double rlog(double base, double x);

int id = 0;

int main() {
	string s;
	cout << "  Use x, y, z variables \n";
	cout << "  Type expression in infix notation ";
	getline(cin, s);
	cout << endl;
	string *tokens = new string[s.length()];
	int size;
	evaluating_tokens(tokens, s, size);
	Node *ExpressionTree = infixToRPN(tokens, size);
	//draw expression tree
	//evaluate operators values
	//calculate expression
	FILE *f = fopen("output.txt", "w+b");
	bst_print_dot(ExpressionTree, f);
	double x, y, z;
	cout << "  x = ";
	cin >> x;
	cout << "  y = ";
	cin >> y;
	cout << "  z = ";
	cin >> z;
	double result = evaluateTree(ExpressionTree, x, y, z);

	cout << "  Result = " << result; 
}

Node *infixToRPN(string *tokens, int size) {
	stack<Node*> operators;
	queue<Node*> output;
	stack<Node*> treenodes;
	string token, token2;

	for (int i = 0; i < size; i++) {
		token = tokens[i];
		if (isOperator(token) || isFunction(token)) {
			if (operators.empty() == false) {
				while (((isFunction(operators.top()->info))
					|| (priority(operators.top()->info) > priority(token))
					|| (priority(operators.top()->info) == priority(token)) && isLeftAssociative(operators.top()->info))
					&& (operators.top()->info != "(")) {
					operators.push(makeNode(token));
					operators.pop();
					if (output.empty() || operators.empty()) break;
				}
			}
			operators.push(makeNode(token));
		}
		else if (token == "(") {
			operators.push(makeNode(token));
		}
		else if (token == ")") {
			while (operators.top()->info != "(") {
				attachOperator(treenodes, operators);
				if (operators.empty()) break;
			}
			if (!operators.empty()) operators.pop(); // delete ( without putting it into stack
			else return false; // no ( found - error
		}
		//else if (token == ",") {
		//	//
		//}
		else { // token is a number
			treenodes.push(makeNode(token));
		}
	}
	while (!operators.empty())
	{
		if (isParenthesis(operators.top()->info))
		{
			return false;
		}
		attachOperator(treenodes, operators);
	}

	return treenodes.top();
}

bool isFunction(string token) {
	if (token == "exp" || token == "log") return true;
	else return false;
}

bool isLeftAssociative(string token) {
	if (token == "+" || token == "-" || token == "*" || token == "/") return true; else return false;
}

void evaluating_tokens(string *tokens, string s, int &index_tokens) {
	string token;
	index_tokens = 0;
	string str = "";
	for (int i = 0; i < s.length(); i++)
	{
		token = s[i];
		if (isOperator(token) || isParenthesis(token))
		{
			if (!str.empty())
			{
				tokens[index_tokens] = str;
				index_tokens++;
			}
			str = "";
			tokens[index_tokens] = token;
			index_tokens++;
		}
		else
		{
			// Append the numbers    
			if (!token.empty() && token != " ")
			{
				if (token != ",") {
					str.append(token);
				}
				else {
					tokens[index_tokens] = str;
					index_tokens++;
					/*str = ",";
					tokens[index_tokens] = str;
					index_tokens++;*/
					str = "";
				}
				
			}
			else
			{
				if (str != "")
				{
					tokens[index_tokens] = str;
					index_tokens++;
					str = "";
				}
			}
		}
	}
	if (str != "") {
		tokens[index_tokens] = str;
		index_tokens++;
	}

}

int priority(string token) {
	if (token == "*") return 2; else
		if (token == "/") return 2; else
			if (token == "+") return 1; else
				if (token == "-") return 1; else
					return 0;
}

bool isParenthesis(string token)
{
	if (token == "(" || token == ")") return true;
	else return false;
}

bool isOperator(string token)
{
	if (token == "+" || token == "-" || token == "*" || token == "/") return true;
	else return false;
}

Node *makeNode(string info) {
	Node *temp = new Node();
	temp->info = info;
	temp->right = NULL;
	temp->left = NULL;
	temp->id = id;
	id++;
	return temp;
}

void attachOperator(stack<Node*> &treenodes, stack<Node*> &operators) {
	Node *operatorNode = operators.top();

	if (operators.top()->info == "exp") {
		operators.pop();
		operatorNode->right = treenodes.top();
		treenodes.pop();
		operatorNode->left = NULL;
		treenodes.push(operatorNode);
	} else {
		operators.pop();
		operatorNode->right = treenodes.top();
		treenodes.pop();
		operatorNode->left = treenodes.top();
		treenodes.pop();
		treenodes.push(operatorNode);
	}
	
}

double evaluateTree(Node *treenode, double x1, double y1, double z1) {
	double x, y, z;
	if (isOperator(treenode->info)) {
		x = evaluateTree(treenode->left, x1, y1, z1);
		y = evaluateTree(treenode->right, x1, y1, z1);
		if (treenode->info == "+")
			z = x + y;
		else if (treenode->info == "-")
			z = x - y;
		else if (treenode->info == "*")
			z = x * y;
		else if (treenode->info == "/")
			z = x / y;
		return z;
	} else if (isFunction(treenode->info)){
		if (treenode->info == "log") {
			x = evaluateTree(treenode->left, x1, y1, z1);
			y = evaluateTree(treenode->right, x1, y1, z1);
			z = rlog(x, y);
			return z;
		}
		else if (treenode->info == "exp") {
			x = evaluateTree(treenode->right, x1, y1, z1);
			z = exp(x);
			return z;
		}
	} else  { 
		if (treenode->info == "x")
			return x1;
		else if (treenode->info == "y")
			return y1;
		else if (treenode->info == "z")
			return z1;
		else {
			int number;
			istringstream num(treenode->info);
			num >> number;
			return number;
		}
	}
}

double rlog(double base, double x) {
	double result = log(x) / log(base);
	return result;
}

//functions for graph visualization in GraphViz
void bst_print_dot_null(Node *key, int nullcount, FILE* stream)
{
	fprintf(stream, "    null%d [shape=point];\n", nullcount);
	fprintf(stream, "    %d [label=\"%s\"];\n", key->id, key->info.c_str());
	fprintf(stream, "    %d -> null%d;\n", key->id, nullcount);
}

void bst_print_dot_aux(Node* node, FILE* stream)
{
	static int nullcount = 0;
	static int dotcount = 0;
	if (node->left)
	{
		fprintf(stream, "    %d [label=\"%s\"];\n", node->id, node->info.c_str());
		fprintf(stream, "    %d -> %d;\n", node->id, node->left->id);
		bst_print_dot_aux(node->left, stream);
	}
	else
		bst_print_dot_null(node, nullcount++, stream);

	if (node->right)
	{
		fprintf(stream, "    %d [label=\"%s\"];\n", node->id, node->info.c_str());
		fprintf(stream, "    %d -> %d;\n", node->id, node->right->id);
		bst_print_dot_aux(node->right, stream);
	}
	else
		bst_print_dot_null(node, nullcount++, stream);
}

void bst_print_dot(Node* tree, FILE* stream)
{
	fprintf(stream, "digraph BST {\n");
	fprintf(stream, "    node [fontname=\"Arial\"];\n");

	if (!tree)
		fprintf(stream, "\n");
	else if (!tree->right && !tree->left) {
		fprintf(stream, "    %d [label=\"%s\"];\n", tree->id, tree->info.c_str());
		fprintf(stream, "    %d;\n", tree->id);
		//fprintf(stream, "    \"%s\";\n", tree->info.c_str());
	}
	else
		bst_print_dot_aux(tree, stream);

	fprintf(stream, "}\n");
}