/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ADBLOCK_RUST_FFI_SRC_WRAPPER_HPP_
#define ADBLOCK_RUST_FFI_SRC_WRAPPER_HPP_

#include <memory>
#include <optional>
#include <string>
#include <vector>

extern "C" {
#include "lib.h"  // NOLINT
}

#if defined(ADBLOCK_SHARED_LIBRARY)
#if defined(WIN32)
#if defined(ADBLOCK_IMPLEMENTATION)
#define ADBLOCK_EXPORT __declspec(dllexport)
#else
#define ADBLOCK_EXPORT __declspec(dllimport)
#endif  // defined(ADBLOCK_IMPLEMENTATION)
#else   // defined(WIN32)
#if defined(ADBLOCK_IMPLEMENTATION)
#define ADBLOCK_EXPORT __attribute__((visibility("default")))
#else
#define ADBLOCK_EXPORT
#endif  // defined(ADBLOCK_IMPLEMENTATION)
#endif
#else  // defined(ADBLOCK_SHARED_LIBRARY)
#define ADBLOCK_EXPORT
#endif

namespace adblock {

typedef C_DomainResolverCallback DomainResolverCallback;

bool ADBLOCK_EXPORT SetDomainResolver(DomainResolverCallback resolver);

class ADBLOCK_EXPORT FilterList {
 public:
  FilterList(const std::string& uuid,
             const std::string& url,
             const std::string& title,
             const std::vector<std::string>& langs,
             const std::string& support_url,
             const std::string& component_id,
             const std::string& base64_public_key,
             const std::string& desc);
  explicit FilterList(const FilterList& other);
  ~FilterList();

  const std::string uuid;
  const std::string url;
  const std::string title;
  const std::vector<std::string> langs;
  const std::string support_url;
  const std::string component_id;
  const std::string base64_public_key;
  const std::string desc;

 private:
  static std::vector<FilterList> default_list;
  static std::vector<FilterList> regional_list;
};

typedef ADBLOCK_EXPORT struct FilterListMetadata {
  FilterListMetadata();
  explicit FilterListMetadata(C_FilterListMetadata* metadata);
  ~FilterListMetadata();

  std::optional<std::string> homepage;
  std::optional<std::string> title;

  FilterListMetadata(FilterListMetadata&&);

  FilterListMetadata(const FilterListMetadata&) = delete;
} FilterListMetadata;


class ADBLOCK_EXPORT FilterSet {
  friend class Engine;
 public:
  FilterSet();
  ~FilterSet();

  FilterSet(FilterSet &&) = default;

  void addFilterList(const std::string &filter_list);

 private:
  FilterSet(const FilterSet &) = delete;
  void operator=(const FilterSet&) = delete;
  C_FilterSet* raw = nullptr;
};

class ADBLOCK_EXPORT Engine {
 public:
  static Engine fromFilterSet(FilterSet *set, bool optimize);
  Engine();
  explicit Engine(C_Engine* c_engine);
  explicit Engine(const std::string& rules);
  Engine(const char* data, size_t data_size);
  void matches(const std::string& url,
               const std::string& host,
               const std::string& tab_host,
               bool is_third_party,
               const std::string& resource_type,
               bool* did_match_rule,
               bool* did_match_exception,
               bool* did_match_important,
               std::string* redirect);
  std::string getCspDirectives(const std::string& url,
                               const std::string& host,
                               const std::string& tab_host,
                               bool is_third_party,
                               const std::string& resource_type);
  bool deserialize(const char* data, size_t data_size);
  void addTag(const std::string& tag);
  void addResource(const std::string& key,
                   const std::string& content_type,
                   const std::string& data);
  void addResources(const std::string& resources);
  void removeTag(const std::string& tag);
  bool tagExists(const std::string& tag);
  const std::string urlCosmeticResources(const std::string& url);
  const std::string hiddenClassIdSelectors(
      const std::vector<std::string>& classes,
      const std::vector<std::string>& ids,
      const std::vector<std::string>& exceptions);
  ~Engine();

  Engine(Engine&&);
  void operator=(Engine&&);

 private:
  Engine(const Engine&) = delete;
  void operator=(const Engine&) = delete;
  C_Engine* raw = nullptr;
};

std::pair<FilterListMetadata, std::unique_ptr<Engine>> engineWithMetadata(
    const std::string& rules);
std::pair<FilterListMetadata, std::unique_ptr<Engine>>
engineFromBufferWithMetadata(const char* data, size_t data_size);

}  // namespace adblock

#endif  // ADBLOCK_RUST_FFI_SRC_WRAPPER_HPP_
