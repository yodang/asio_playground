#include <iostream>
#include <string>
#include <future>
#include <cstddef>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <boost/program_options.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace asio=boost::asio;
namespace signal2=boost::signals2;

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

class session
{
public:
    session(asio::ip::tcp::socket&& sock)
        : sock_(std::move(sock)),
        data_avail(),
        rx_buffer_(std::make_unique<std::array<uint8_t, 256>>())
    {

        async_read(sock_, asio::mutable_buffer(rx_buffer_->data(), rx_buffer_->max_size()), [&](auto ec, auto n){data_read(ec, n);});
    }
    
    //Returns a future which will contain the number of bytes written
    std::future<int> send(const std::vector<uint8_t> mess)
    {
        async_write(sock_, asio::buffer(mess.data(), mess.size()), [](auto ec, auto n){std::cout<<n<<" bytes written"<<std::endl;});
    }

    signal2::connection receive_connect(std::function<void(uint8_t[])> f)
    {
        return data_avail.connect(f);
    }
private:
    void data_read(boost::system::error_code ec, std::size_t n)
    {
        std::cout<<"code: "<<ec<<std::endl;
        std::cout<<n<<" bytes available"<<std::endl;
        //if(sock_.is_open())
        //    async_read(sock_, asio::mutable_buffer(rx_buffer_->data(), rx_buffer_->size()), [&](auto ec, auto n){data_read(ec, n);});
    }
    signal2::signal<void(uint8_t[])> data_avail;
    asio::ip::tcp::socket sock_;
    std::unique_ptr<std::array<uint8_t, 256>> rx_buffer_;
};

class client
{
public:
    client(const std::shared_ptr<asio::io_service>& ctx)
        :ctx_(ctx)
    {
    }

    session connect(const std::string host)
    {
        bool res=false;
        asio::ip::tcp::resolver r(*ctx_);
        auto sock=asio::ip::tcp::socket(*ctx_);
        

        //try{
        auto endpoints=r.resolve(asio::ip::tcp::resolver::query(host, "6667"));
        asio::connect(sock, endpoints);
        //}
        //catch(std::system_error err){
        //    std::cerr<<err.what();
        //    return res;
        //}
        //
        std::cout<<"connected to "<<host<<std::endl;
        return session(std::move(sock));
    }


private:
    std::shared_ptr<asio::io_service> ctx_;
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
    auto guard=asio::make_work_guard(context);

    try{
    session s=c.connect(option_map["host"].as<std::string>());
    context->run();
    }
    catch(std::system_error e)
    {
        std::cerr<<e.what()<<std::endl;
    }
    //

    return 0;
}
