#include <platform/events.hpp>

static EventQueue<EngineEvent, false> engine_event_queue;
static EventQueue<PlatformEvent, false> platform_event_queue;
static EventQueue<ResourceEvent, true> resource_event_queue;
static EventQueue<AudioEvent, true> audio_event_queue;

void pde_events_push_platform_event(PlatformEvent event) { platform_event_queue.push_event(event); }
void pde_events_push_engine_event(EngineEvent event) { engine_event_queue.push_event(event); }
void pde_events_push_resource_event(ResourceEvent event) { resource_event_queue.push_event(event); }
void pde_events_push_audio_event(AudioEvent event) { audio_event_queue.push_event(event); }
int32_t pde_events_pop_platform_event(PlatformEvent *event) { return platform_event_queue.pop_event(event); }
int32_t pde_events_pop_engine_event(EngineEvent *event) { return engine_event_queue.pop_event(event); }
int32_t pde_events_pop_resource_event(ResourceEvent *event) { return resource_event_queue.pop_event(event); }
int32_t pde_events_pop_audio_event(AudioEvent *event) { return audio_event_queue.pop_event(event); }
void pde_events_swap_platform_event_queue() { platform_event_queue.swap_queues(); }
void pde_events_swap_engine_event_queue() { engine_event_queue.swap_queues(); }
void pde_events_swap_resource_event_queue() { resource_event_queue.swap_queues(); }
void pde_events_swap_audio_event_queue() { audio_event_queue.swap_queues(); }
void pde_events_wait_for_engine_events() { engine_event_queue.wait_for_events(); }
void pde_events_wait_for_platform_events() { platform_event_queue.wait_for_events(); }
void pde_events_wait_for_resource_events() { resource_event_queue.wait_for_events(); }
void pde_events_wait_for_audio_events() { audio_event_queue.wait_for_events(); }
