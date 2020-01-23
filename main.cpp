#include <iostream>
#include "webserver.h"
#include "Socket.h"
#include "appdynamics-cpp-sdk\include\appdynamics.h"

const char APP_NAME[] = "Techno ZOO";
const char TIER_NAME[] = "Local.CPP.Sever";
const char NODE_NAME[] = "Local.CPP.Sever-01";
const char CONTROLLER_HOST[] = "appd-controller.lab.sw.local";
const int CONTROLLER_PORT = 8090;
const char CONTROLLER_ACCOUNT[] = "customer1";
const char CONTROLLER_ACCESS_KEY[] = "739e6c2a-d5d2-4549-84f5-9061055ba1bc";
const int CONTROLLER_USE_SSL = 0; 

int init_appd()
{
  struct appd_config* cfg = appd_config_init(); // appd_config_init() resets the configuration object and pass back an handle/pointer
  appd_config_set_app_name(cfg, APP_NAME);
  appd_config_set_tier_name(cfg, TIER_NAME);
  appd_config_set_node_name(cfg, NODE_NAME);
  appd_config_set_controller_host(cfg, CONTROLLER_HOST);
  appd_config_set_controller_port(cfg, CONTROLLER_PORT);
  appd_config_set_controller_account(cfg, CONTROLLER_ACCOUNT);
  appd_config_set_controller_access_key(cfg, CONTROLLER_ACCESS_KEY);
  appd_config_set_controller_use_ssl(cfg, CONTROLLER_USE_SSL);

  int initRC = appd_sdk_init(cfg);
  
  if (initRC) {
        std::cerr <<  "Error: sdk init: " << initRC << std::endl;
        return -1;
  }  
}

void Request_Handler(webserver::http_request* r) 
{   
  // start the "/" transaction
    
  Socket s = *(r->s_);

  std::string title;
  std::string body;
  std::string bgcolor="#ffffff";
  std::string links =
      "<p><a href='/red'>red</a> "
      "<br><a href='/blue'>blue</a> "
      "<br><a href='/form'>form</a> "
      "<br><a href='/auth'>authentication example</a> [use <b>rene</b> as username and <b>secretGarden</b> as password"
      "<br><a href='/header'>show some HTTP header details</a> "
      ;



  std::cout << "Requested: " << r->path_ << " Method: " << r->method_  << std::endl;
  if(r->path_ == "/") {
    appd_bt_handle btHandle = appd_bt_begin("/", NULL);
    title = "Web Server Example";
    body  = "<h1>Welcome to Rene's Web Server</h1>"
            "I wonder what you're going to click"  + links;
    // end the transaction
    appd_bt_end(btHandle);  
  }
  else if (r->path_ == "/red") {
    appd_bt_handle btHandle = appd_bt_begin("/red", NULL);
    bgcolor = "#ff4444";
    title   = "You chose red";
    body    = "<h1>Red</h1>" + links;
    appd_bt_end(btHandle);  
  }
  else if (r->path_ == "/blue") {
    appd_bt_handle btHandle = appd_bt_begin("/blue", NULL);
    bgcolor = "#4444ff";
    title   = "You chose blue";
    body    = "<h1>Blue</h1>" + links;
    appd_bt_end(btHandle);  
  }
  else if (r->path_ == "/form") {
    appd_bt_handle btHandle = appd_bt_begin("/form", NULL);
    title   = "Fill a form";

    body    = "<h1>Fill a form</h1>";
    body   += "<form action='/form'>"
              "<table>"
              "<tr><td>Field 1</td><td><input name=field_1></td></tr>"
              "<tr><td>Field 2</td><td><input name=field_2></td></tr>"
              "<tr><td>Field 3</td><td><input name=field_3></td></tr>"
              "</table>"
              "<input type=submit></form>";


    for (std::map<std::string, std::string>::const_iterator i = r->params_.begin();
         i != r->params_.end();
         i++) {

      body += "<br>" + i->first + " = " + i->second;
    }


    body += "<hr>" + links;

    appd_bt_end(btHandle);  
  }
  else if (r->path_ == "/auth") {
    appd_bt_handle btHandle = appd_bt_begin("/auth", NULL);
    if (r->authentication_given_) {
      if (r->username_ == "rene" && r->password_ == "secretGarden") {
         body = "<h1>Successfully authenticated</h1>" + links;
      }
      else {
         body = "<h1>Wrong username or password</h1>" + links;
         r->auth_realm_ = "Private Stuff";
      }
    }
    else {
      r->auth_realm_ = "Private Stuff";
    }
    appd_bt_end(btHandle);  
  }
  else if (r->path_ == "/header") {
    appd_bt_handle btHandle = appd_bt_begin("/header", NULL);
    title   = "some HTTP header details";
    body    = std::string ("<table>")                                   +
              "<tr><td>Accept:</td><td>"          + r->accept_          + "</td></tr>" +
              "<tr><td>Accept-Encoding:</td><td>" + r->accept_encoding_ + "</td></tr>" +
              "<tr><td>Accept-Language:</td><td>" + r->accept_language_ + "</td></tr>" +
              "<tr><td>User-Agent:</td><td>"      + r->user_agent_      + "</td></tr>" +
              "</table>"                                                +
              links;
    appd_bt_end(btHandle);  
  }
  else {
    r->status_ = "404 Not Found";
    title      = "Wrong URL";
    body       = "<h1>Wrong URL</h1>";
    body      += "Path is : &gt;" + r->path_ + "&lt;"; 

    std::cout << "Wrong URL: \'" << r->path_ << "\'" << std::endl;
  }

  r->answer_  = "<html><head><title>";
  r->answer_ += title;
  r->answer_ += "</title></head><body bgcolor='" + bgcolor + "'>";
  r->answer_ += body;
  r->answer_ += "</body></html>";
 
}

int main() 
{
  init_appd();
  
  webserver(8080, Request_Handler);

  appd_sdk_term();  
}
