#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "visitor.h"

#define printm(...) printf(__VA_ARGS__)
#define printb(...) printf(__VA_ARGS__)

FILE *fp;
int tempVars = 0;        // variável para indicar o indice atual das variáveis temporárias

void visit_file (AST *root) {
	printm(">>> file\n");
	fp = fopen("test_result.ll", "w");
	printm("file has %d declarations\n", root->list.num_items);
	
	// prints filename to IR file
	fprintf(fp, "source_filename = \"%s\"\n", root->list.first->ast->decl.function.token->filename);

	for (ListNode *ptr = root->list.first; ptr != NULL; ptr = ptr->next) {
		switch (ptr->ast->decl.type) {
		case FUNCTION_DECLARATION:
			visit_function_decl(ptr->ast); break;
		case VARIABLE_DECLARATION:
			visit_var_decl(ptr->ast); break;
		default:
			fprintf(stderr, "UNKNOWN DECLARATION TYPE %c\n", ptr->ast->decl.type);
			break;
		}
	}
	printm("<<< file\n");
	fclose(fp);
}

void visit_function_decl (AST *ast) {
	printm(">>> function_decl\n");
	AST *params = ast->decl.function.param_decl_list;
	
	fprintf(fp, "\n\ndefine ");
	switch (ast->decl.function.type)
	{
	case TYPE_VOID:
		fprintf(fp, "void ");
		break;
	case TYPE_INT:
		fprintf(fp, "i32 ");
	default:
		break;
	}
	
	fprintf(fp, "@%s(", ast->decl.function.id->id.string);

	if (params != NULL) {
		for (int i = 1; i <= params->list.num_items; i++) {
			if (i == params->list.num_items)
			{
				fprintf(fp, "i32");
			}
			else
			{
				fprintf(fp, "i32, ");
			}
			tempVars++;
			printm("param ");
		}
		printm("\n");
	}
	fprintf(fp, ") #0 {");
	if (ast->decl.function.stat_block != NULL) {
		visit_stat_block(ast->decl.function.stat_block, params, ast->decl.function.type);
	}
	fprintf(fp, "\n}");
	tempVars = 0;
	printm("<<< function_decl\n");
}

// what is surrounded by { }
ExprResult visit_stat_block (AST *stat_block, AST *params, int return_type) {
	printm(">>> stat_block\n");
	ExprResult ret = { 0, TYPE_VOID };

	for (ListNode *ptr = stat_block->list.first; ptr != NULL; ptr = ptr->next) {
		if(ptr->ast->stat.type == VARIABLE_DECLARATION)
		{
			AST *id = ptr->ast->decl.variable.id;
			tempVars++;
			sprintf(id->id.string, "%d", tempVars);
			fprintf(fp, "\n  %%%d = alloca i32, align 4", tempVars);
		}
	}
	for (ListNode *ptr = stat_block->list.first; ptr != NULL; ptr = ptr->next) {
		ret = visit_stat(ptr->ast);
	}
	printm("<<< stat_block\n");
	return  ret;
}


ExprResult visit_stat (AST *stat) {
	printm(">>> statement\n");
	ExprResult ret = { 0, TYPE_VOID };
	switch (stat->stat.type) {
	case VARIABLE_DECLARATION:
		visit_var_decl_local(stat); break;
	case ASSIGN_STATEMENT:
		visit_assign_stat(stat); break;
	case RETURN_STATEMENT:
		ret = visit_return_stat(stat); break;
	case EXPRESSION_STATEMENT:
		visit_expr(stat->stat.expr.expr); break;
		default: fprintf(stderr, "UNKNOWN STATEMENT TYPE %c\n", stat->stat.type); break;
	}
	printm("<<< statement\n");
	return ret;
}

void visit_var_decl (AST *ast) {
	printm(">>> var_decl\n");
	AST *id = ast->decl.variable.id;	
	
	if (ast->decl.variable.expr != NULL) {
		ExprResult expr = visit_expr(ast->decl.variable.expr);
		id->id.int_value = expr.int_value;
		fprintf(fp, "\n@%s = local_unnamed_addr global i32 %ld, align 4", id->id.string, id->id.int_value);
	}
	else
	{
		fprintf(fp, "\n@%s = common local_unnamed_addr global i32 0, align 4", id->id.string);
	}
	printm("<<< var_decl\n");
}

void visit_var_decl_local (AST *ast) {
	printm(">>> var_decl_local\n");
	AST *id = ast->decl.variable.id;

	if (ast->decl.variable.expr != NULL) {
		ExprResult expr = visit_expr(ast->decl.variable.expr);
		id->id.int_value = expr.int_value;
		fprintf(fp, "\n  store i32 %ld, i32* %%%s, align 4", id->id.int_value, id->id.string);
	}
	printm("<<< var_decl_local\n");
}

ExprResult visit_return_stat (AST *ast) {
	printm(">>> return stat\n");
	ExprResult ret = { 0, TYPE_VOID };
	if (ast->stat.ret.expr) {
		ret = visit_expr(ast->stat.ret.expr);
	}
	printm("<<< return stat\n");
	return ret;
}

void visit_assign_stat (AST *assign) {
	printm(">>> assign stat\n");
	ExprResult expr = visit_expr(assign->stat.assign.expr);
	assign->id.int_value = expr.int_value;
	printm("<<< assign stat\n");
}

ExprResult visit_expr (AST *expr) {
	printm(">>> expression\n");
	ExprResult ret = {};
	switch (expr->expr.type) {
	case BINARY_EXPRESSION:
		switch (expr->expr.binary_expr.operation) {
		case '+':
			ret = visit_add(expr); break;
		case '-':
			ret = visit_sub(expr); break;
		case '*':
			ret = visit_mul(expr); break;
		case '/':
			ret = visit_div(expr); break;
		case '%':
			ret = visit_mod(expr); break;
		default:
			fprintf(stderr, "UNKNOWN OPERATOR %c\n", expr->expr.binary_expr.operation); break;
		}
		break;
	case UNARY_MINUS_EXPRESSION:
		ret = visit_unary_minus(expr); break;
	case LITERAL_EXPRESSION:
		ret = visit_literal(expr); break;
	case IDENTIFIER_EXPRESSION:
		ret = visit_id(expr->expr.id.id); break;
	case FUNCTION_CALL_EXPRESSION:
		ret = visit_function_call(expr); break;
	default:
		fprintf(stderr, "UNKNOWN EXPRESSION TYPE %c\n", expr->expr.type);
		break;
	}
	printm("<<< expression\n");
	return ret;
}

// não implementar
ExprResult visit_function_call (AST *ast) {
	printm(">>> function_call\n");
	ExprResult ret = {}, arg_expr[20]; //instead of alloca
	AST *arg_list = ast->expr.function_call.expr_list;

	if (arg_list != NULL) {
		int i = 0;
		for (ListNode *ptr = arg_list->list.first; ptr != NULL; ptr = ptr->next) {
			arg_expr[i++] = visit_expr(ptr->ast);
		}
	}
	printm("<<< function_call\n");
	return ret;
}

ExprResult visit_id (AST *ast) {
	printm(">>> identifier\n");
	ExprResult ret = {}; // armazenar aqui
	if (ast->id.type == TYPE_INT) {
		ret.int_value = ast->id.int_value;
		ret.type = TYPE_INT;
		tempVars++;
		fprintf(fp, "\n  %%%d = load i32, i32* %%%s, align 4", tempVars, ast->id.string);
	} else if (ast->id.type == TYPE_FLOAT) {
		ret.float_value = ast->id.float_value;
		ret.type = TYPE_FLOAT;
	}
	printm("<<< identifier\n");
	return ret;
}

ExprResult visit_literal (AST *ast) {
	printm(">>> literal\n");
	ExprResult ret = {};
	ret.int_value = ast->expr.literal.int_value;
	printm("<<< literal\n");
	return ret;
}

ExprResult visit_unary_minus (AST *ast) {
	printm(">>> unary_minus\n");
	ExprResult expr, ret = {};
	expr = visit_expr(ast->expr.unary_minus.expr);
	ret.int_value = -expr.int_value;
	printm("<<< unary_minus\n");
	return ret;
}

ExprResult visit_add (AST *ast) {
	printm(">>> add\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	ret.int_value = left.int_value + right.int_value;

		if (left.type == FLOAT_CONSTANT)
	{
		tempVars++;
		if (right.type == FLOAT_CONSTANT)
		{
			fprintf(fp, "\n  %%%d = add nsw i32 %%%s, %%%s", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
		}
		else
		{
			fprintf(fp, "\n  %%%d = add nsw i32 %%%s, %ld", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, right.int_value);
		}
	}
	else if(right.type == FLOAT_CONSTANT)
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = add nsw i32 %ld, %%%s", tempVars, left.int_value, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
	}
	printm("<<< add\n");
	return ret;
}

ExprResult visit_sub (AST *ast) {
	printm(">>> sub\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	ret.int_value = left.int_value - right.int_value;

	if (left.type == FLOAT_CONSTANT)
	{
		tempVars++;
		if (right.type == FLOAT_CONSTANT)
		{
			fprintf(fp, "\n  %%%d = sub nsw i32 %%%s, %%%s", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
		}
		else
		{
			fprintf(fp, "\n  %%%d = sub nsw i32 %%%s, %ld", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, right.int_value);
		}
	}
	else if(right.type == FLOAT_CONSTANT)
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = sub nsw i32 %ld, %%%s", tempVars, left.int_value, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
	}
	printm("<<< sub\n");
	return ret;
}

ExprResult visit_mul (AST *ast) {
	printm(">>> mul\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	ret.int_value = left.int_value * right.int_value;
	if (left.type == FLOAT_CONSTANT)
	{
		tempVars++;
		if (right.type == FLOAT_CONSTANT)
		{
			fprintf(fp, "\n  %%%d = mul i32 %%%s, %%%s", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
		}
		else
		{
			fprintf(fp, "\n  %%%d = mul i32 %%%s, %ld", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, right.int_value);
		}
	}
	else if(right.type == FLOAT_CONSTANT)
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = mul i32 %ld, %%%s", tempVars, left.int_value, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
	}

	printm("<<< mul\n");
	return ret;
}

ExprResult visit_div (AST *ast) {
	printm(">>> div\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	if (right.int_value != 0)
	{
		ret.int_value = left.int_value % right.int_value;	
	}

	if (left.type == FLOAT_CONSTANT)
	{
		tempVars++;
		if (right.type == FLOAT_CONSTANT)
		{
			fprintf(fp, "\n  %%%d = sdiv i32 %%%s, %%%s", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
		}
		else
		{
			fprintf(fp, "\n  %%%d = sdiv i32 %%%s, %ld", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, right.int_value);
		}
	}
	else if(right.type == FLOAT_CONSTANT)
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = sdiv i32 %ld, %%%s", tempVars, left.int_value, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
	}
	printm("<<< div\n");
	return ret;
}

ExprResult visit_mod (AST *ast) {
	printm(">>> mod\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	
	if (right.int_value != 0)
	{
		ret.int_value = left.int_value % right.int_value;	
	}

	if (left.type == FLOAT_CONSTANT)
	{
		tempVars++;
		if (right.type == FLOAT_CONSTANT)
		{
			fprintf(fp, "\n  %%%d = srem i32 %%%s, %%%s", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
		}
		else
		{
			fprintf(fp, "\n  %%%d = srem i32 %%%s, %ld", tempVars, ast->expr.binary_expr.left_expr->expr.id.id->id.string, right.int_value);
		}
	}
	else if(right.type == FLOAT_CONSTANT)
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = srem i32 %ld, %%%s", tempVars, left.int_value, ast->expr.binary_expr.right_expr->expr.id.id->id.string);
	}
	
	printm("<<< mod\n");
	return ret;
}

