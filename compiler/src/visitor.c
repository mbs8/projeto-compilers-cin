#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "visitor.h"

#define printm(...) printf(__VA_ARGS__)
#define printb(...) printf(__VA_ARGS__)

#define set_flag(a, b) ((a) |= (b))
#define clear_flag(a, b) ((a) &= ~(b))
#define get_flag(a, b) ((a) & (b))

FILE *fp;
int tempVars = 0;        // variável para indicar o indice atual das variáveis temporárias

ExprResult print_operations (ExprResult left, ExprResult right, ExprResult ret, char* operation) {
	if (left.type == INTEGER_CONSTANT && right.type == INTEGER_CONSTANT)
	{
		ret.type = INTEGER_CONSTANT;
	}
	else 
	{
		tempVars++;
		ret.ssa_register = tempVars;
		ret.type = LLIR_REGISTER;
		fprintf(fp, "\n  %%%d = %s i32 ", tempVars, operation);
		if (left.type == INTEGER_CONSTANT)
		{
			fprintf(fp, "%ld,", left.int_value);
		}
		else
		{
			fprintf(fp, "%%%ld,", left.ssa_register);
		}
		if (right.type == INTEGER_CONSTANT)
		{
			fprintf(fp, " %ld", right.int_value);
		}
		else
		{
			fprintf(fp, " %%%ld", right.ssa_register);
		}
		
	}

	return ret;
}

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
	ExprResult ret = { 0, VOID_CONSTANT };

	for (ListNode *ptr = stat_block->list.first; ptr != NULL; ptr = ptr->next) {
		if(ptr->ast->stat.type == VARIABLE_DECLARATION)
		{
			AST *id = ptr->ast->decl.variable.id;
			tempVars++;
			id->id.ssa_register = tempVars;
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
	ExprResult ret = { 0, VOID_CONSTANT };
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
		fprintf(fp, "\n@%s = global i32 %ld, align 4", id->id.string, id->id.int_value);
	}
	else
	{
		fprintf(fp, "\n@%s = common global i32 0, align 4", id->id.string);
	}
	printm("<<< var_decl\n");
}

void visit_var_decl_local (AST *ast) {
	printm(">>> var_decl_local\n");
	AST *id = ast->decl.variable.id;

	if (ast->decl.variable.expr != NULL) {
		ExprResult expr = visit_expr(ast->decl.variable.expr);
		if (expr.type == INTEGER_CONSTANT)
		{
			fprintf(fp, "\n  store i32 %ld, i32* ", expr.int_value);
		}
		else if(expr.type == LLIR_REGISTER) 
		{
			fprintf(fp, "\n  store i32 %%%ld, i32* ", expr.ssa_register);
		}
		if (get_flag(ast->id.flags, IS_GLOBAL))
		{
			fprintf(fp, "@%s, align 4", id->id.string);
		}
		else 
		{
			fprintf(fp, "%%%ld, align 4", id->id.ssa_register);
		}
		
	}
	else 
	{
		fprintf(fp, "\n  store i32 0, i32* %%%ld, align 4", id->id.ssa_register);
	}
	
	printm("<<< var_decl_local\n");
}

ExprResult visit_return_stat (AST *ast) {
	printm(">>> return stat\n");
	ExprResult ret = { 0, VOID_CONSTANT };
	if (ast->stat.ret.expr) {
		ret = visit_expr(ast->stat.ret.expr);
		if (ret.type == INTEGER_CONSTANT)
		{
			fprintf(fp, "\n  ret i32 %ld", ret.int_value);
		}
		else
		{
			fprintf(fp, "\n  ret i32 %%%d", tempVars);
		}
	}
	printm("<<< return stat\n");
	return ret;
}

void visit_assign_stat (AST *assign) {
	printm(">>> assign stat\n");
	ExprResult expr = visit_expr(assign->stat.assign.expr);
	if (get_flag(assign->stat.assign.id->id.flags, IS_GLOBAL))
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = alloca i32, align4", tempVars);
	}
	// if (get_flag(assign->))
	// {
	// 	/* code */
	// }
	
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

	//fprintf(fp, "\n  call ")
	if (ast->expr.function_call.id->id.type == TYPE_VOID)
	{
		fprintf(fp, "\n  call void @%s(", ast->expr.function_call.id->id.string);
	}
	else 
	{
		tempVars++;
		fprintf(fp, "\n  %%%d = call i32 @%s (", tempVars, ast->expr.function_call.id->id.string);
	}
	
	// if ()
	// {
	// 	/* code */
	// }
	
	if (arg_list != NULL) {
		int i = 0;
		for (ListNode *ptr = arg_list->list.first; ptr != NULL; ptr = ptr->next) {
			arg_expr[i++] = visit_expr(ptr->ast);
			if (ptr->next == NULL)
			{
				fprintf(fp, "i32 %ld", arg_expr[i].ssa_register);
			}
			else
			{
				fprintf(fp, "i32 %ld, ", arg_expr[i].ssa_register);
			}
		}
	}
	fprintf(fp, ")");
	printm("<<< function_call\n");
	return ret;
}

ExprResult visit_id (AST *ast) {
	printm(">>> identifier\n");
	ExprResult ret = {}; // armazenar aqui
	
	tempVars++;
	ret.ssa_register = tempVars;
	ret.type = LLIR_REGISTER;
	
	fprintf(fp, "\n  %%%d = load i32, i32* ", tempVars);
	if (get_flag(ast->id.flags, IS_GLOBAL))
	{
		fprintf(fp, "@%s, align 4", ast->id.string);
	}
	else
	{
		fprintf(fp, "%%%ld, align 4", ast->id.ssa_register);
	}
	printm("<<< identifier\n");
	return ret;
}

ExprResult visit_literal (AST *ast) {
	printm(">>> literal\n");
	ExprResult ret = {};
	ret.int_value = ast->expr.literal.int_value;
	ret.type = INTEGER_CONSTANT;
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
	print_operations(left, right, ret, "add nsw");
	printm("<<< add\n");
	return ret;
}

ExprResult visit_sub (AST *ast) {
	printm(">>> sub\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);

	ret.int_value = left.int_value - right.int_value;	
	ret = print_operations(left, right, ret, "sub nsw");
	printm("<<< sub\n");
	return ret;
}

ExprResult  visit_mul (AST *ast) {
	printm(">>> mul\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	
	ret.int_value = left.int_value * right.int_value;	
	ret = print_operations(left, right, ret, "mul");
	printm("<<< mul\n");
	return ret;
}

ExprResult visit_div (AST *ast) {
	printm(">>> div\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	
	//if (right.int_value != 0)
	//{
		ret.int_value = left.int_value % right.int_value;	
	//}
	printm("<<< div\n");
	return ret;
}

ExprResult visit_mod (AST *ast) {
	printm(">>> mod\n");
	ExprResult left, right, ret = {};
	left  = visit_expr(ast->expr.binary_expr.left_expr);
	right = visit_expr(ast->expr.binary_expr.right_expr);
	
	//if (right.int_value != 0)
	//{
		ret.int_value = left.int_value % right.int_value;	
	//}

	print_operations(left, right, ret, "srem");
	printm("<<< mod\n");
	return ret;
}


