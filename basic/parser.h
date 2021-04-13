#ifndef PARSER_H
#define PARSER_H
#include "QStack"
#include "expression.h"
enum Type_e {P_VAR,P_NUM,P_LBR,P_RBR,P_OPS,P_DEFAULT};
class parser
{
public:
    parser();
    ~parser();
    Expression* buildTree(QString);
    void clear();
private:
    QStack<QString> operators;
    QStack<Expression*> operands;
    int getPrecedence(QString);
    void popOps();
};

#endif // PARSER_H
