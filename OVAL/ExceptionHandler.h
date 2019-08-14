#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <QString>
#include <exception>

//TODO: look into QErrorMessage Class

///@brief - Handles any disruptions to the normal flow of the program.
class ExceptionHandler
{
public:
    //TODO: Use this as a key to a map of checks, os that it can be used to request a check for a specific error.
    enum ErrorType { UNDEFINED, INDEX_OUT_OF_RANGE, MEMEORY_ACCESS_VIOLATION, DATA_TYPE_ERROR};

    ExceptionHandler();

    void DisplayErrorMessage(QString errorMessage, ErrorType errorType);

    /**
     * @brief TryCatch Runs code and checks for a set error.
     *
     * The function runs off a switch case on ErrorType.
     *
     * @param errorType
     * @return
     */
    bool TryCatch(QString dataType, QString data);
};

#endif // EXCEPTIONHANDLER_H
