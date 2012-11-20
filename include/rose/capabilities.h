#ifndef ROSE_CAPABILITIES_H
#define ROSE_CAPABILITIES_H

int capabilities_is_64bit(void);
int capabilities_has_apic(void);
int capabilities_has_pae(void);
int capabilities_has_pse(void);

#endif
