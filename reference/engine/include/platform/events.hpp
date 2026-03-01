#pragma once
#include <cstdint>
#include <platform/events/platform_events.hpp>
#include <platform/events/engine_events.hpp>
#include <platform/events/audio_events.hpp>
#include <platform/events/resource_events.hpp>

void pde_events_push_platform_event(PlatformEvent event);
void pde_events_push_engine_event(EngineEvent event);
void pde_events_push_resource_event(ResourceEvent event);
void pde_events_push_audio_event(AudioEvent event);

int32_t pde_events_pop_platform_event(PlatformEvent *event);
int32_t pde_events_pop_engine_event(EngineEvent *event);
int32_t pde_events_pop_resource_event(ResourceEvent *event);
int32_t pde_events_pop_audio_event(AudioEvent *event);

void pde_events_swap_platform_event_queue();
void pde_events_swap_engine_event_queue();
void pde_events_swap_resource_event_queue();
void pde_events_swap_audio_event_queue();

void pde_events_wait_for_engine_events();
void pde_events_wait_for_platform_events();
void pde_events_wait_for_resource_events();
void pde_events_wait_for_audio_events();
