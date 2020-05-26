/* Status Handler Header*/
#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include "request_handler.h"

class StatusRequestHandler : public RequestHandler  {

	public:
		StatusRequestHandler(const NginxConfig &config);

		static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

		std::shared_ptr<reply> HandleRequest(const request& request_) override;  

	private:
		std::string display_content;
		std::string request_handler_info;
		std::string request_info;
};

#endif