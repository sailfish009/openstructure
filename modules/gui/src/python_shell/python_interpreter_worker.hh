#ifndef PYTHON_INTERPRETER_WORKER_HH
#define PYTHON_INTERPRETER_WORKER_HH

#include <csignal>
#include <utility>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include "output_redirector.hh"
#include <QObject>
#include <QQueue>

namespace ost { namespace gui {
namespace bp = boost::python;

class PythonInterpreterWorker: public QObject
{
Q_OBJECT
public:
  PythonInterpreterWorker();
  unsigned int AddCommand(const QString& command);

signals:
  void Finished(unsigned int id, bool error_state);
  void Output(unsigned int id,const QString& output);
  void ErrorOutput(unsigned int id,const QString& output);

public slots:
  void Wake();

protected slots:
  void handle_redirector_output(const QString& output);
  void handle_redirector_error(const QString& output);

protected:
  bool is_simple_expression(const QString& expr);
  void run_command_(std::pair<unsigned int,QString> pair);
  bool is_simple_expression_(const QString& expr);
  QQueue<std::pair<unsigned int,QString> > exec_queue_;
  unsigned int command_id_;
  boost::shared_ptr<OutputRedirector>  output_redirector_;
  boost::shared_ptr<OutputRedirector>  error_redirector_;
#ifndef _MSC_VER
  struct sigaction sig_act_;
#endif
  bp::object parse_expr_cmd_;
  bp::object repr_;
  bp::dict main_namespace_;
  unsigned int current_id_;
};

}} //ns

#endif // PYTHON_INTERPRETER_WORKER_HH
