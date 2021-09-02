#!python3

import os, sys, pkg_resources;

# Grab directory
l_hasAllDependencies = True;
l_directoryPath = os.path.dirname(os.path.realpath(__file__));

# Check all dependencies in list
with open( l_directoryPath + "/pythonDependencyList.txt" ) as dependency_listing_file:
	for line in dependency_listing_file:
		package = " ".join(line.splitlines())
		try:
			dist = pkg_resources.get_distribution(package)
			print( "{} ({}) is installed".format(dist.key, dist.version) )
		except pkg_resources.DistributionNotFound:
			print( "{} is NOT installed".format(package) )
			l_hasAllDependencies = False;

# Return sucess status
sys.exit(0 if l_hasAllDependencies else -1)