#include "Parser.hpp"
#include "Node.hpp"
#include "StackAllocator.hpp"
#include <iostream>

bool isObject(FloridaType inType){
	switch(inType){
		case Bool:
			return true;
		case fixed1:
			return true;
		case fixed2:
			return true;
		case fixed4:
			return true;
		case fixed8:
			return true;
		default:
			return false;
	}
}

Node* Parser::parse(){
    //If it's not the nullptr, then it's successful.
    return this->program();
};

//List of all statements that can exist in a program.
Program* Parser::programList(){
    Node* thisNode = nullptr;

    //Check for a jump statement.
    thisNode = jump();
    if(thisNode != nullptr){
        return stack->alloc<Program>(thisNode);
    }

    //Check for an if statement.
    thisNode = If();
    if(thisNode != nullptr){
        return stack->alloc<Program>(thisNode);
    }

    //Check for an assignment.
    thisNode = assignment();
    if(thisNode != nullptr){
        return stack->alloc<Program>(thisNode);
    }

    //Check for a landing point (for jump statements).
    thisNode = landing();
    if(thisNode != nullptr){
        return stack->alloc<Program>(thisNode);
    }

    // //Check for an expression.
    // thisNode = p0();
    // if(thisNode != nullptr){
    //     return stack->alloc<Program>(thisNode);
    // }

    //If nothing works, then there's an error. Return a nullptr.
    return nullptr;

}

Node* Parser::program(){
    Program* thisProgram = nullptr;
    Program* start = stack->alloc<Program>(nullptr);
    Program* current = start;

    //Get the first statement in the program.
    current->head = thisProgram = programList();
    int statement = 1;

    //Extremely basic debugger.
    if(thisProgram == nullptr){
        std::cout << "There has been an error in the parser.\nCheck the " + std::to_string(statement) + "th statement.\n";
        return nullptr;
    }

    //Get all statements in the program.
    while(thisProgram != nullptr){  
        //If all tokens have been consumed, then terminate parsing.
        if(given.size() == iter){
            break;
        }
        thisProgram = nullptr;

        //Check for another statement.
        //A nullptr means there has been an error.
        thisProgram = programList();
        statement++;
        if(thisProgram != nullptr){
            current->Append(thisProgram);
            current = current->next;
        }
    }

    return start;

}

Node* Parser::assignment(){
    Node* left = nullptr;        
    Node* right = nullptr;
    Node* initial = stack->peek<Node>();
    int current = iter;

    left = initialize();
    //Check for and increment for the semicolon if there is one.
    if(left != nullptr && given[iter].name == ";"){
        iter++;
        return left;
    }

    //If it is not an initialization, then it should be a variable.
    if(left == nullptr){
        left = variable();
    }

    //If it's still a nullptr, then it's not an assignment at all.
    //Rewind the parser and try for another statement.
    if(left == nullptr){
        return nullptr;
    }

    //if an assignment is found
    if(given[iter].name == "="){
        iter++;
        //Increment the iterator, and then get p0().
        right = p0();
        if(right != nullptr){
            //Check for and increment for the semicolon if there is one.
            //Otherwise, there is an error.
            if(given[iter].name == ";"){
                iter++;
                return stack->alloc<Assignment>(left, right);
            } else {
                stack->dealloc(left);
                std::cout << "Error: [Line: " + std::to_string(given[iter - 1].row) + "]\n\tMissing semicolon.\n";
                return nullptr;
            }
        } else {
            std::cout << "Error: [Line: " + std::to_string(given[iter - 1].row) + "]\n\tNo assignable expression was found.\n";
            stack->dealloc(initial);
            return nullptr;
        }
    } 

    //Increment for the semicolon, otherwise there's an error.
    if(given[iter].name == ";"){
        iter++;
        return left;
    }

    stack->dealloc(initial);
    iter = current;
    return nullptr;

}

Node* Parser::variable(){
    //Check for the edge case that it is a landing point in code.
    if(given[iter].type == FloridaType::Identifier && given[iter + 1].type != FloridaType::Identifier && given[iter + 1].name != ":"){
        //Increment for the variable.
        iter++;
        return stack->alloc<Variable>(given[iter - 1].name);
    } else {
        return nullptr;
    }
}

Node* Parser::initialize(){
    //Check to see if there is an object and an identifier,
    //otherwise it is not an initialization.
    if(isObject(given[iter].type) && given[iter + 1].type == FloridaType::Identifier){
        //Increment for the adjective.
        iter++;
        //Increment for the variable name.
        iter++;
        return stack->alloc<Initialize>(given[iter - 2].name, given[iter - 1].name);
    } else {
        return nullptr;
    }

}

Node* Parser::jump(){
    if((given[iter].name == "goto") && (given[iter + 1].type == FloridaType::Identifier) && given[iter + 2].name == ";"){
        //Increment for seeing goto.
        iter++;
        //Increment for seeing the landing line.
        iter++;
        //Increment for the semicolon.
        iter++;

        return stack->alloc<Goto>(given[iter - 2].name);

    }

    //Something went wrong somewhere.
    return nullptr;
}

Node* Parser::If(){
    Node* current = stack->peek<Node>();

    if(given[iter].name == "if" && given[iter + 1].name == "("){
        //Increment for the if and (.
        iter++;
        iter++;

        Node* condition = p0();
        //Increment for the ).
        iter++;
        Program* body = nullptr;

        //Check to see if the condition is a statement.
        if(condition == nullptr){
            return nullptr;
        }

        //Check for the body.   
        if(given[iter].name == "{"){
            iter++;

            body = programList();
            Program* secondBody = body;
            //Check for all statements.
            while(secondBody != nullptr){
                secondBody->Append(programList());
                secondBody = secondBody->next;
            }

            return stack->alloc<IfObject>(condition, body);

        }

        //If nothing works, this isn't the in the correct format.
        stack->dealloc(current);
        return nullptr;

    }

    return nullptr;

}



Node* Parser::landing(){
    if((given[iter].type == FloridaType::Identifier) & (given[iter + 1].name == ":")){
        //Increment for the identifier.
        iter++;
        //Increment for the colon.
        iter++;

        return stack->alloc<Landing>(given[iter - 2].name);

    }

    //Something has gone wrong.
    return nullptr;
}


//Mathy stuff
Node* Parser::p0(){
    Node* left = p1();
    std::string current = given[iter].getName();

    //Check for + or - operators between p1() expressions.
    while(current == "+" || current == "-"){
        iter++;
        Node* right = p1();
        //Check for any errors in the subexpression.
        if(right == nullptr){
            return nullptr;
        }
        if(current == "+"){
            left = stack->alloc<Add>(left, right);
            current = given[iter].getName();
            continue;
        }
        if(current == "-"){
            left = stack->alloc<Subtract>(left, right);
            current = given[iter].getName();
            continue;
        }
    }

    return left;

}

Node* Parser::p1(){
    Node* left = p2();
    std::string current = given[iter].getName();

    while(current == "*" || current == "/"){
        iter++;
        Node* right = p2();
        if(current == "*"){
            left = stack->alloc<Multiply>(left, right);
            current = given[iter].getName();
            continue;
        }
        if(current == "/"){
            left = stack->alloc<Divide>(left, right);
            current = given[iter].getName();
            continue;
        }
    }

    return left;

}

Node* Parser::p2(){
    //Check for negations
    std::string current = given[iter].getName();
    if(current == "-"){
        Node* expression;
        //Increment for the minus sign;
        iter++;
        expression = stack->alloc<Negative>(p1());

        return expression;
    }
    //Check for expression within parentheses.
    if(current == "("){
        Node* expression;
        //Increment for the left parenthesis;
        iter++;
        expression = stack->alloc<Parentheses>(p0());
        //Increment for the right parenthesis;
        iter++;

        return expression;
    }
    //Check for variables.
    if(given[iter].getType() == FloridaType::Identifier){
        Variable* thisVariable = stack->alloc<Variable>(given[iter].getName());
        iter++;     

        return thisVariable;
    }
    //Check for numbers.
    if(given[iter].type == FloridaType::fixed8){ 
        Fixed8* number = stack->alloc<Fixed8>(given[iter].getName());
        iter++;

        return number;
    }
    //Check for booleans.
    if(given[iter].type == FloridaType::Bool){
        if(given[iter].name == "True"){
            iter++;
            return stack->alloc<Boolean>(true);
        } else {
            iter++;
            return stack->alloc<Boolean>(false);
        }
    }

    return nullptr;
}



