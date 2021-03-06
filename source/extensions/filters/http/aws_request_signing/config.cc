#include "extensions/filters/http/aws_request_signing/config.h"

#include "envoy/extensions/filters/http/aws_request_signing/v3/aws_request_signing.pb.h"
#include "envoy/extensions/filters/http/aws_request_signing/v3/aws_request_signing.pb.validate.h"
#include "envoy/registry/registry.h"

#include "extensions/filters/http/aws_request_signing/aws_request_signing_filter.h"
#include "extensions/filters/http/common/aws/credentials_provider_impl.h"
#include "extensions/filters/http/common/aws/signer_impl.h"
#include "extensions/filters/http/common/aws/utility.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace AwsRequestSigningFilter {

Http::FilterFactoryCb AwsRequestSigningFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::aws_request_signing::v3::AwsRequestSigning& config,
    const std::string& stats_prefix, Server::Configuration::FactoryContext& context) {

  auto credentials_provider =
      std::make_shared<HttpFilters::Common::Aws::DefaultCredentialsProviderChain>(
          context.api(), HttpFilters::Common::Aws::Utility::metadataFetcher);
  auto signer = std::make_unique<HttpFilters::Common::Aws::SignerImpl>(
      config.service_name(), config.region(), credentials_provider,
      context.dispatcher().timeSource());

  auto filter_config =
      std::make_shared<FilterConfigImpl>(std::move(signer), stats_prefix, context.scope());
  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    auto filter = std::make_shared<Filter>(filter_config);
    callbacks.addStreamFilter(filter);
  };
}

/**
 * Static registration for the AWS request signing filter. @see RegisterFactory.
 */
REGISTER_FACTORY(AwsRequestSigningFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace AwsRequestSigningFilter
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
