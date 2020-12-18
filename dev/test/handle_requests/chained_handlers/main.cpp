/*
	restinio
*/

#include <catch2/catch.hpp>

#include <restinio/all.hpp>
#include <restinio/chained_handlers/fixed_size.hpp>

#include <test/common/utest_logger.hpp>
#include <test/common/pub.hpp>

#include "../../common/test_user_data_factory.ipp"

template<
	typename Request_Handler,
	typename User_Data_Factory >
struct test_traits_t : public restinio::traits_t<
	restinio::asio_timer_manager_t, utest_logger_t >
{
	using request_handler_t = Request_Handler;
	using user_data_factory_t = User_Data_Factory;
};

template< typename User_Data_Factory >
void
tc_fixed_size_chain()
{
	using http_server_t = restinio::http_server_t<
			test_traits_t<
					restinio::chained_handlers::fixed_size_chain_t<
							4u, User_Data_Factory>,
					User_Data_Factory >
	>;

	int stages_completed = 0;

	http_server_t http_server{
		restinio::own_io_context(),
		[&stages_completed]( auto & settings ){
			settings
				.port( utest_default_port() )
				.address( "127.0.0.1" )
				.request_handler(
					[&stages_completed]( auto /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( auto req ) {
						++stages_completed;

						req->create_response()
							.append_header( "Server", "RESTinio utest server" )
							.append_header_date_field()
							.append_header( "Content-Type", "text/plain; charset=utf-8" )
							.set_body(
								restinio::const_buffer( req->header().method().c_str() ) )
							.done();

						return restinio::request_accepted();
					} );
		} };

	other_work_thread_for_server_t<http_server_t> other_thread(http_server);
	other_thread.run();

	std::string response;
	const char * request_str =
		"GET / HTTP/1.1\r\n"
		"Host: 127.0.0.1\r\n"
		"User-Agent: unit-test\r\n"
		"Accept: */*\r\n"
		"Connection: close\r\n"
		"\r\n";

	REQUIRE_NOTHROW( response = do_request( request_str ) );

	REQUIRE_THAT( response, Catch::Matchers::EndsWith( "GET" ) );

	other_thread.stop_and_join();

	REQUIRE( 4 == stages_completed );
}

TEST_CASE( "fixed_size_chain (no_user_data)" ,
		"[fixed_size_chain][no_user_data]" )
{
	tc_fixed_size_chain< restinio::no_user_data_factory_t >();
}

TEST_CASE( "fixed_size_chain (test_user_data)" ,
		"[fixed_size_chain][test_user_data]" )
{
	tc_fixed_size_chain< test::ud_factory_t >();
}

template< typename User_Data_Factory >
void
tc_fixed_size_chain_with_rejection()
{
	using http_server_t = restinio::http_server_t<
			test_traits_t<
					restinio::chained_handlers::fixed_size_chain_t<
							4u, User_Data_Factory>,
					User_Data_Factory >
	>;

	int stages_completed = 0;

	http_server_t http_server{
		restinio::own_io_context(),
		[&stages_completed]( auto & settings ){
			settings
				.port( utest_default_port() )
				.address( "127.0.0.1" )
				.request_handler(
					[&stages_completed]( auto /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_rejected();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( auto req ) {
						++stages_completed;

						req->create_response()
							.append_header( "Server", "RESTinio utest server" )
							.append_header_date_field()
							.append_header( "Content-Type", "text/plain; charset=utf-8" )
							.set_body(
								restinio::const_buffer( req->header().method().c_str() ) )
							.done();

						return restinio::request_accepted();
					} );
		} };

	other_work_thread_for_server_t<http_server_t> other_thread(http_server);
	other_thread.run();

	std::string response;
	const char * request_str =
		"GET / HTTP/1.1\r\n"
		"Host: 127.0.0.1\r\n"
		"User-Agent: unit-test\r\n"
		"Accept: */*\r\n"
		"Connection: close\r\n"
		"\r\n";

	REQUIRE_NOTHROW( response = do_request( request_str ) );

	REQUIRE_THAT( response,
			Catch::Matchers::StartsWith( "HTTP/1.1 501 Not Implemented" ) );

	other_thread.stop_and_join();

	REQUIRE( 2 == stages_completed );
}

TEST_CASE( "fixed_size_chain_with_rejection (no_user_data)" ,
		"[fixed_size_chain][no_user_data]" )
{
	tc_fixed_size_chain_with_rejection< restinio::no_user_data_factory_t >();
}

TEST_CASE( "fixed_size_chain_with_rejection (test_user_data)" ,
		"[fixed_size_chain][test_user_data]" )
{
	tc_fixed_size_chain_with_rejection< test::ud_factory_t >();
}

template< typename User_Data_Factory >
void
tc_fixed_size_chain_accept_in_middle()
{
	using http_server_t = restinio::http_server_t<
			test_traits_t<
					restinio::chained_handlers::fixed_size_chain_t<
							4u, User_Data_Factory>,
					User_Data_Factory >
	>;

	int stages_completed = 0;

	http_server_t http_server{
		restinio::own_io_context(),
		[&stages_completed]( auto & settings ){
			settings
				.port( utest_default_port() )
				.address( "127.0.0.1" )
				.request_handler(
					[&stages_completed]( auto /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					},
					[&stages_completed]( auto req ) {
						++stages_completed;

						req->create_response()
							.append_header( "Server", "RESTinio utest server" )
							.append_header_date_field()
							.append_header( "Content-Type", "text/plain; charset=utf-8" )
							.set_body(
								restinio::const_buffer( req->header().method().c_str() ) )
							.done();

						return restinio::request_accepted();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_rejected();
					},
					[&stages_completed]( const auto & /*req*/ ) {
						++stages_completed;
						return restinio::request_not_handled();
					} );
		} };

	other_work_thread_for_server_t<http_server_t> other_thread(http_server);
	other_thread.run();

	std::string response;
	const char * request_str =
		"GET / HTTP/1.1\r\n"
		"Host: 127.0.0.1\r\n"
		"User-Agent: unit-test\r\n"
		"Accept: */*\r\n"
		"Connection: close\r\n"
		"\r\n";

	REQUIRE_NOTHROW( response = do_request( request_str ) );

	REQUIRE_THAT( response, Catch::Matchers::EndsWith( "GET" ) );

	other_thread.stop_and_join();

	REQUIRE( 2 == stages_completed );
}

TEST_CASE( "fixed_size_chain_accept_in_middle (no_user_data)" ,
		"[fixed_size_chain][no_user_data]" )
{
	tc_fixed_size_chain_accept_in_middle< restinio::no_user_data_factory_t >();
}

TEST_CASE( "fixed_size_chain_accept_in_middle (test_user_data)" ,
		"[fixed_size_chain][test_user_data]" )
{
	tc_fixed_size_chain_accept_in_middle< test::ud_factory_t >();
}

