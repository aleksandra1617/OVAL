#include "ExceptionHandler.h"
#include <iostream>
#include <QMessageBox>

ExceptionHandler::ExceptionHandler()
{

}

void ExceptionHandler::DisplayErrorMessage(QString errorMessage, ErrorType errorType)
{
    QMessageBox messageBox;
    messageBox.critical(0, "ERROR", errorMessage);
    messageBox.setFixedSize(300,150);
}


bool ExceptionHandler::TryCatch(QString dataType, QString data)
{
    try
    {
        if (dataType == "int")
            std::stoi(data.toStdString());
        else if (dataType == "float")
            std::stof(data.toStdString());
        else if((dataType == "bool") && ((data != "false" && data !="true")))
            throw "Invalid conversion from string to boolean!";
    }
    catch (std::invalid_argument &e)
    {
        DisplayErrorMessage("Invalid data type!", ErrorType::DATA_TYPE_ERROR);
        return false;
    }
    catch(const char* msg)
    {
        DisplayErrorMessage(msg, ErrorType::DATA_TYPE_ERROR);
        return false;
    }
    catch (std::exception &e)
    {
        DisplayErrorMessage("Exception Thrown("+QString::fromStdString(e.what())+") when atempting data type conversion.",
                            ErrorType::DATA_TYPE_ERROR);
        return false;
    }

    return true;
}
