require 'mxx_ru/cpp'
require 'restinio/asio_helper.rb'

MxxRu::Cpp::exe_target {

	RestinioAsioHelper.attach_propper_asio( self )

	required_prj 'nodejs/llhttp_mxxru/prj.rb'
	required_prj 'fmt_mxxru/prj.rb'
	required_prj 'restinio/platform_specific_libs.rb'

	target( "_test.router.easy_parser_router_bench" )

	cpp_source( "main.cpp" )
}
