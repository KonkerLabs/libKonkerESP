Import("env")
import time, json
# import platform,subprocess

def storeFWInfo(source, target, env):
	content = {"version":"0.0.0", "sequence_number":str(round(time.time() * 1000)), "device":"S000"}

	with open("data/firmware_info.json", "w") as cred:
		cred.write(json.dumps(content))

    # tool = 'esptool' if platform.system().lower()=='windows' else 'esptool.py'
	# command = [tool, '--no-stub', '--after', 'soft_reset', 'write_flash', '0xFD000', 'firmware_info.json']
	# p = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    #
	# if p.returncode != 0:
	# 	print("Error setting platform credential")
	# 	sys.exit()
    #
	# print("FW information stored")
    #
	# # Delete temporary generated binary file
	# os.remove('firmware_info.json')

# def before_upload(source, target, env):
#     print "before_upload"
    # do some actions

# def after_upload(source, target, env):
#     print "after_upload"
    # do some actions

env.AddPreAction("upload", storeFWInfo)
# env.AddPostAction("upload", storeFWInfo)
