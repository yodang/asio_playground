#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include <boost/program_options.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace asio=boost::asio;

void timer_cb(const boost::system::error_code&)
{
    std::cout << "Hello World!" << std::endl;
}

class frame
{
public:
    frame()
    {

    }
};

class client
{
public:
    client(const std::shared_ptr<asio::io_service>& ctx)
        :ctx_(ctx),
        sock_(nullptr)
    {
    }

    bool connect(const std::string host)
    {
        bool res=false;
        asio::ip::tcp::resolver r(*ctx_);
        auto sock=std::make_unique<asio::ip::tcp::socket>(*ctx_);
        

        try{
            auto endpoints=r.resolve(asio::ip::tcp::resolver::query(host, "6667"));
            asio::connect(*sock, endpoints);
            res=true;
        }
        catch(std::system_error err){
            std::cerr<<err.what();
            return res;
        }
        
        sock_.swap(sock);
        std::cout<<"connected to "<<host<<std::endl;
        return res;
    }

    void send();

private:
    std::shared_ptr<asio::io_service> ctx_;
    std::unique_ptr<asio::ip::tcp::socket> sock_;
};

using namespace boost::program_options;

int main(int argc, char** argv)
{
    options_description desc("Usage: ");
    desc.add_options()
        ("help", "Print this message")
        ("host", value<std::string>(), "Host to connect to")
        ("port", value<std::string>()->default_value("6667"), "Port to connect to")
    ;

    variables_map option_map;
    store(parse_command_line(argc, argv, desc), option_map);
    notify(option_map);

    if((option_map.count("host")!=1) || option_map.count("help"))
    {
        std::cout<<desc<<std::endl;
        return 0;
    }

    auto context=std::make_shared<asio::io_service>();
    client c(context);

    c.connect(option_map["host"].as<std::string>());

    //context->run();

    return 0;
}