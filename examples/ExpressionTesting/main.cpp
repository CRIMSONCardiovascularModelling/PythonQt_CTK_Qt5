// NOTE: This only works on release builds

#include "PythonQt.h"
#include <QApplication>
#include <QTextBrowser>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>

#include <QTextStream>

#include <stdio.h>

const char* const TestScript =
"def takesInArray(array):       \n"
"    total = 0                  \n"
"    for item in array:         \n"
"        total += item          \n"
"    return total";

static QVariant DebugPyObject(const char* const script)
{
    qDebug("attempting to evaluate");
    qDebug(script);


    // get the __main__ python module
    PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();

    // according to this https://mevislab.github.io/pythonqt/Developer.html
    // it should be a QVariantMap for dicts
    QVariant result = mainModule.evalScript(script, Py_eval_input);

    if(result.isValid())
    {
        qDebug("Is valid");
    }
    else
    {
        qDebug("Is not valid");
    }

    qDebug(result.toString().toStdString().c_str());

    return result;
}

int main( int argc, char **argv )
{
  //printf("Hello! Press enter to continue!\n");
  //getchar();
  QApplication qapp(argc, argv);

  // init PythonQt and Python
  PythonQt::init();

  printf("Initialized PythonQt!\n");

  QVariant intResult = DebugPyObject("2+2");

  QVariant listResult = DebugPyObject("[0,1,2,3,4]");



  QVariant strListResult = DebugPyObject("['zero', 'one', 'two','three']");

  QList<QVariant> varList_strList = strListResult.value<QList<QVariant>>();

  std::vector<std::string> strs;
  for(const QVariant& variant : varList_strList)
  {
    QString qStr = variant.value<QString>();
    std::string sStr = qStr.toStdString();
    strs.push_back(sStr);
  }

  qDebug("Contents of array:");
  for(const std::string& sStr : strs)
  {
      qDebug(sStr.c_str());
  }

  qDebug("\n");

  QVariant exceptionResult = DebugPyObject("raise RuntimeError('hi there')");

  // get the __main__ python module
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();

  // according to this https://mevislab.github.io/pythonqt/Developer.html
  // it should be a QVariantMap
  // which is typedef QMap<QString, QVariant> QVariantMap;

  {
      QVariant dictResult = DebugPyObject("{'one':1, 'two':2}");
      QMap<QString, QVariant> qMap = qvariant_cast<QMap<QString, QVariant>>(dictResult);

      const QList<QString> keys = qMap.keys();

      qDebug("Dictionary entries:" );
      for(const QString& key : keys)
      {
          QVariant value = qMap[key];
          QString entryString = QString("%1:%2").arg(key).arg(value.toString());
          qDebug(entryString.toStdString().c_str());
      }
  }

  qDebug();


  {
      QVariant dictResult_intKey = DebugPyObject("{1:'one', 2:'two'}");

      // QMap<QVariant, QVariant> does not seem to be a valid cast from a dictionary result
      QMap<QString, QVariant> qMap = qvariant_cast<QMap<QString, QVariant>>(dictResult_intKey);

      const QList<QString> keys = qMap.keys();

      qDebug("Dictionary entries:" );
      for(const QString& key : keys)
      {
          int keyValue = key.toInt();
          QVariant value = qMap[key];
          QString entryString = QString("%1:%2").arg(keyValue).arg(value.toString());
          qDebug(entryString.toStdString().c_str());
      }
  }

  qDebug();

  printf("Got mainmodule!\n");

  // evaluate a simple python script and receive the result a qvariant:
  QVariant result = mainModule.evalScript("19*2+4", Py_eval_input);

  qDebug("The result of the script is ");
  qDebug(result.toString().toStdString().c_str());

  // define a python method that appends the passed text to the browser
  mainModule.evalScript("def appendText(text):\n  return text + 'hello'");
  // shows how to call the method with a text that will be append to the browser
  QVariant result2 = mainModule.call("appendText", QVariantList() << "Here's what the python script appended: ");

  qDebug(result2.toString().toStdString().c_str());

  /*
  printf("Attempting to run script file\n");

  // evaluate the python script which is defined in the resources
  // this crashes the program
  //mainModule.evalFile("C:/_FILES/home/qtproj/build-MyPythonQtTestProj-Desktop_Qt_5_7_0_MSVC2013_64bit-Release/release/GettingStarted.py");

  // it just does nothing if the file doesn't exist.
  //mainModule.evalFile("pickles.py");

  printf("Evaluated script file!\n");

  // gets nothing
  QVariant result3 = mainModule.getVariable("SomePythonGlobal");

  qDebug("SomePythonGlobal is");
  qDebug(result3.toString().toStdString().c_str());

  QVariant result4 = mainModule.evalScript("SomePythonGlobal", Py_eval_input);

  qDebug("SomePythonGlobal (cheese) is");
  qDebug(result4.toString().toStdString().c_str());
  */

  mainModule.evalScript(TestScript);

  // note that QVariantList is just QList<QVariant>, but I'm concerned there's some extra meaning to this
  QVariantList parameters;
  QVariant a = QVariant::fromValue(22);
  QVariant b = QVariant::fromValue(35);
  QVariantList items = {a,b};

  // It will let you append a QVariantList to a QVariantList, but it doesn't actually wrap it in a QVariant for you
  // and it will not actually be able to do anything with it.
  //
  // I would not have seen that coming.
  QVariant items_Variant = QVariant::fromValue(items);
  parameters.append(items_Variant);

  int type = items_Variant .userType();
  qDebug("UserType is %d\n", type);
  if(type == QMetaType::QVariantList)
  {
      qDebug("Items is a QVariantList");
  }


  QVariant arrayResult = mainModule.call("takesInArray", parameters);

  qDebug("Array result:");
  qDebug(arrayResult.toString().toStdString().c_str());



  printf("Got to the end!\n");

  return qapp.exec();
}

