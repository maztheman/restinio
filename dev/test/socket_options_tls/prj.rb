require 'mxx_ru/cpp'
require 'restinio/asio_helper.rb'

MxxRu::Cpp::exe_target {

	RestinioAsioHelper.attach_propper_asio( self )

	required_prj 'nodejs/llhttp_mxxru/prj.rb'
	required_prj 'fmt_mxxru/prj.rb'
	required_prj 'restinio/platform_specific_libs.rb'
	required_prj 'restinio/open_ssl_libs.rb'
	required_prj 'test/catch_main/prj.rb'

	target( "_unit.test.socket_options_tls" )

	cpp_source( "main.cpp" )
}

