#include <QWebSocketServer>
#include <QWebSocket>

int main(int, char**) {
  QWebSocketServer wss("", QWebSocketServer::NonSecureMode);
  return 0;
}
