"""
GitHub Sync Helper for Line Maze Solver
Repo: https://github.com/padmanaban1908/line-maze-solver.git
Directly syncs and updates code, ideas, and other project files via GitHub REST API.
"""

import os
import sys
import json
import base64
import getpass
import urllib.request
import urllib.error

GITHUB_API_URL = "https://api.github.com"
DEFAULT_REPO_OWNER = "padmanaban1908"
DEFAULT_REPO_NAME = "line-maze-solver"


def get_files_to_sync(root_dir):
    ignored_patterns = {".git", "__pycache__", ".vscode", ".pio", ".venv", "venv", ".idea"}
    files = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Exclude ignored directories
        dirnames[:] = [d for d in dirnames if d not in ignored_patterns]
        for f in filenames:
            if f.endswith((".pyc", ".log", ".tmp")):
                continue
            full_path = os.path.join(dirpath, f)
            rel_path = os.path.relpath(full_path, root_dir).replace("\\", "/")
            files.append((rel_path, full_path))
    return files


def verify_repo(token, owner, repo):
    headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "User-Agent": "LineMazeSolver-Sync",
    }
    url = f"{GITHUB_API_URL}/repos/{owner}/{repo}"
    try:
        req = urllib.request.Request(url, headers=headers)
        with urllib.request.urlopen(req) as resp:
            data = json.loads(resp.read().decode("utf-8"))
            print(f"✅ Authenticated successfully! Connected to: {data.get('html_url')}")
            return True
    except urllib.error.HTTPError as e:
        if e.code == 401:
            print("❌ Authentication failed (401 Bad Credentials): Please verify your token.")
        elif e.code == 404:
            print(f"❌ Repository '{owner}/{repo}' not found or token lacks 'repo' permissions.")
        else:
            print(f"❌ GitHub API Error ({e.code}): {e.read().decode('utf-8')}")
        return False


def upload_file(token, owner, repo, rel_path, local_path, message="Update line maze solver"):
    headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "User-Agent": "LineMazeSolver-Sync",
    }

    with open(local_path, "rb") as f:
        content_bytes = f.read()
        content_b64 = base64.b64encode(content_bytes).decode("utf-8")

    url = f"{GITHUB_API_URL}/repos/{owner}/{repo}/contents/{rel_path}"

    # Check if file exists to fetch sha (for update vs create)
    sha = None
    try:
        check_req = urllib.request.Request(url, headers=headers)
        with urllib.request.urlopen(check_req) as resp:
            existing = json.loads(resp.read().decode("utf-8"))
            sha = existing.get("sha")
    except urllib.error.HTTPError:
        pass

    payload = {
        "message": message,
        "content": content_b64,
        "branch": "main",
    }
    if sha:
        payload["sha"] = sha

    put_req = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers=headers,
        method="PUT",
    )

    try:
        with urllib.request.urlopen(put_req) as resp:
            status = "Updated" if sha else "Created"
            print(f"  [✓] {status}: {rel_path}")
            return True
    except urllib.error.HTTPError as e:
        print(f"  [✗] Failed to sync {rel_path}: {e.read().decode('utf-8')}")
        return False


def sync_all(token, owner=DEFAULT_REPO_OWNER, repo=DEFAULT_REPO_NAME):
    root_dir = os.path.dirname(os.path.abspath(__file__))

    print("=" * 65)
    print(f"🚀 SYNCING TO GITHUB: https://github.com/{owner}/{repo}")
    print("=" * 65)

    if not verify_repo(token, owner, repo):
        sys.exit(1)

    files = get_files_to_sync(root_dir)
    print(f"\n📦 Found {len(files)} files to upload across code, ideas, and others:\n")

    success_count = 0
    for rel_path, full_path in files:
        if upload_file(token, owner, repo, rel_path, full_path, message=f"Sync {rel_path}"):
            success_count += 1

    print("\n" + "=" * 65)
    print(f"🎉 SYNC COMPLETE: {success_count}/{len(files)} files uploaded!")
    print(f"🔗 View your repository live at: https://github.com/{owner}/{repo}")
    print("=" * 65)


if __name__ == "__main__":
    # Load from .env if present
    env_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), ".env")
    if os.path.exists(env_path):
        with open(env_path, "r", encoding="utf-8") as f:
            for line in f:
                if line.strip().startswith("GITHUB_TOKEN="):
                    os.environ["GITHUB_TOKEN"] = line.strip().split("=", 1)[1]

    token = os.getenv("GITHUB_TOKEN")

    if not token and len(sys.argv) > 1:
        token = sys.argv[1].strip()

    if not token:
        print("\nGitHub Token not provided.")
        print("To push to https://github.com/padmanaban1908/line-maze-solver:")
        print("1. Generate a token at: https://github.com/settings/tokens/new?scopes=repo&description=line-maze-solver")
        try:
            token = getpass.getpass("Enter your GitHub Personal Access Token (PAT): ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nAborted.")
            sys.exit(1)

    if not token:
        print("Token cannot be empty.")
        sys.exit(1)

    sync_all(token)
